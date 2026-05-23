#pragma once

namespace Hooks {
void Install();

struct TESCameraSetState {
    [[nodiscard]] static bool Install();
    static void thunk(RE::TESCamera* a_camera, RE::TESCameraState* a_state);

    static inline REL::Relocation<decltype(thunk)> func;
};
}
