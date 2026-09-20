#include "Hooks.h"

#include <SKSE/SKSE.h>

#include <RE/P/PlayerCamera.h>
#include <RE/T/TESCamera.h>
#include <RE/T/TESCameraState.h>
#include <REL/Pattern.h>
#include <REL/Relocation.h>
#include <xbyak/xbyak.h>

#include <array>
#include <cstdint>
#include <cstring>
#include <span>

namespace {
// mov [rsp + 8], rbx is one complete instruction on the supported runtimes.
constexpr std::array<std::uint8_t, 5> kSetStatePrologue {0x48, 0x89, 0x5C, 0x24, 0x08};

struct TESCameraSetState {
    static void Thunk(RE::TESCamera* a_camera, RE::TESCameraState* a_state) {
        auto* playerCamera = RE::PlayerCamera::GetSingleton();
        if ((playerCamera != nullptr)
            && (a_camera == playerCamera)
            && (a_state != nullptr)
            && (a_state->id == RE::CameraState::kAutoVanity)) {
            auto& cameraData = playerCamera->GetRuntimeData2();
            cameraData.allowAutoVanityMode = false;
            cameraData.idleTimer = 0.0F;
            SKSE::log::debug("Blocked the player camera's auto-vanity transition.");
            return;
        }

        original(a_camera, a_state);
    }

    static inline REL::Relocation<decltype(Thunk)> original;
};

void InstallPrologueHook(std::uintptr_t a_address) {
    struct Patch : Xbyak::CodeGenerator {
        explicit Patch(std::uintptr_t a_originalAddress) {
            for (const auto byte : kSetStatePrologue) {
                db(byte);
            }
            jmp(ptr[rip]);
            dq(a_originalAddress + kSetStatePrologue.size());
        }
    };

    Patch patch(a_address);
    patch.ready();

    auto& trampoline = SKSE::GetTrampoline();
    auto* gateway = trampoline.allocate(patch.getSize());
    std::memcpy(gateway, patch.getCode(), patch.getSize());
    TESCameraSetState::original = reinterpret_cast<std::uintptr_t>(gateway);
    trampoline.write_branch<kSetStatePrologue.size()>(a_address, TESCameraSetState::Thunk);
}
}

namespace Hooks {
void Install() {
    const REL::Relocation<const std::uint8_t*> target {RELOCATION_ID(32290, 33026)};
    const auto address = target.address();

    if (REL::make_pattern<"E9">().match(address)) {
        TESCameraSetState::original = SKSE::GetTrampoline().write_branch<kSetStatePrologue.size()>(
            address,
            TESCameraSetState::Thunk
        );
        SKSE::log::info("Chained the existing TESCamera::SetState hook.");
        return;
    }

    if (std::memcmp(target.get(), kSetStatePrologue.data(), kSetStatePrologue.size()) == 0) {
        InstallPrologueHook(address);
        SKSE::log::info("Installed the TESCamera::SetState hook.");
        return;
    }

    const std::span<const std::uint8_t, 8> bytes(target.get(), 8);
    SKSE::log::critical(
        "Cannot hook TESCamera::SetState at {:X}: unexpected bytes {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X}. "
        "Vanity camera suppression is unavailable.",
        address,
        bytes[0],
        bytes[1],
        bytes[2],
        bytes[3],
        bytes[4],
        bytes[5],
        bytes[6],
        bytes[7]
    );
}
}
