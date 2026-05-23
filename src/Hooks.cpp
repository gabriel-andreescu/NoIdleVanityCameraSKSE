#include "Hooks.h"
#include "VanityCamera.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include <xbyak/xbyak.h>

namespace {
constexpr std::size_t kSetStatePatchSize = 5;

constexpr std::array<std::byte, kSetStatePatchSize> kTESCameraSetStatePrologue {
    std::byte {0x48},
    std::byte {0x89},
    std::byte {0x5C},
    std::byte {0x24},
    std::byte {0x08},
};

[[nodiscard]] bool HasExpectedTESCameraSetStatePrologue(const std::byte* a_address) noexcept {
    return std::memcmp(a_address, kTESCameraSetStatePrologue.data(), kTESCameraSetStatePrologue.size()) == 0;
}

template <class T, std::size_t BYTES>
void HookFunctionPrologue(const std::uintptr_t a_src, const std::byte* a_originalBytes) {
    struct Patch : Xbyak::CodeGenerator {
        Patch(
            const std::uintptr_t a_originalFuncAddr,
            const std::byte* a_originalBytes,
            const std::size_t a_originalByteLength
        ) {
            for (::std::size_t i = 0; i < a_originalByteLength; ++i) {
                db(::std::to_integer<::std::uint8_t>(a_originalBytes[i]));
            }

            jmp(ptr[rip]);
            dq(a_originalFuncAddr + a_originalByteLength);
        }
    };

    Patch patch(a_src, a_originalBytes, BYTES);
    patch.ready();

    auto& trampoline = SKSE::GetTrampoline();
    trampoline.write_branch<5>(a_src, T::thunk);

    const auto alloc = trampoline.allocate(patch.getSize());
    std::memcpy(alloc, patch.getCode(), patch.getSize());

    T::func = reinterpret_cast<std::uintptr_t>(alloc);
}

void LogUnsupportedTESCameraSetStatePrologue(const std::uintptr_t a_address, const std::byte* a_bytes) {
    logger::critical(
        "Hooks: TESCamera::SetState hook skipped | reason=unsupportedPrologue | address={:X} | bytes={:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X}",
        a_address,
        std::to_integer<unsigned>(a_bytes[0]),
        std::to_integer<unsigned>(a_bytes[1]),
        std::to_integer<unsigned>(a_bytes[2]),
        std::to_integer<unsigned>(a_bytes[3]),
        std::to_integer<unsigned>(a_bytes[4]),
        std::to_integer<unsigned>(a_bytes[5]),
        std::to_integer<unsigned>(a_bytes[6]),
        std::to_integer<unsigned>(a_bytes[7])
    );
}

[[nodiscard]] bool IsPlayerAutoVanityState(RE::TESCamera* a_camera, RE::TESCameraState* a_state) noexcept {
    return a_camera
           && (a_camera == RE::PlayerCamera::GetSingleton())
           && a_state
           && (a_state->id == RE::CameraState::kAutoVanity);
}
}

namespace Hooks {
void Install() {
    if (!TESCameraSetState::Install()) {
        logger::critical("Hooks: install failed | reason=setStateHook");
        return;
    }

    logger::info("Hooks: installed");
}

bool TESCameraSetState::Install() {
    REL::Relocation<std::byte*> target {RELOCATION_ID(32290, 33026)};
    const auto* targetBytes = target.get();
    const auto address = target.address();
    auto& trampoline = SKSE::GetTrampoline();

    if (REL::make_pattern<"E9">().match(address)) {
        func = trampoline.write_branch<kSetStatePatchSize>(address, thunk);

        logger::warn("Hooks: TESCamera::SetState hook chained | reason=existingBranch | branch=E9");
        return true;
    }

    if (HasExpectedTESCameraSetStatePrologue(targetBytes)) {
        HookFunctionPrologue<TESCameraSetState, kSetStatePatchSize>(address, targetBytes);

        logger::info("Hooks: TESCamera::SetState hook installed");
        return true;
    }

    LogUnsupportedTESCameraSetStatePrologue(address, targetBytes);
    return false;
}

void TESCameraSetState::thunk(RE::TESCamera* a_camera, RE::TESCameraState* a_state) {
    if (IsPlayerAutoVanityState(a_camera, a_state)) {
        VanityCamera::Disable("TESCamera::SetState");
        logger::debug("Hooks: TESCamera::SetState blocked | state=AutoVanity");
        return;
    }

    func(a_camera, a_state);
}
}
