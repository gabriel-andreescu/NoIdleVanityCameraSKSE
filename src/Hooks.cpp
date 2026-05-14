#include "Hooks.h"
#include "VanityCamera.h"

namespace Hooks {
void Install() {
    stl::write_thunk_call<PlayerCameraUpdate>(
        REL::Relocation {RELOCATION_ID(49852, 50784), REL::Relocate(0x1A6, 0x1A6)}
    );

    logger::info("Hooks: installed");
}

void PlayerCameraUpdate::thunk(RE::PlayerCamera* a_camera) {
    VanityCamera::Disable("PlayerCamera::Update pre");
    func(a_camera);
    VanityCamera::Disable("PlayerCamera::Update post");
}
}
