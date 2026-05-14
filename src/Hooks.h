#pragma once

namespace Hooks {
void Install();

struct PlayerCameraUpdate {
    static void thunk(RE::PlayerCamera* a_camera);

    static inline REL::Relocation<decltype(thunk)> func;
};
}
