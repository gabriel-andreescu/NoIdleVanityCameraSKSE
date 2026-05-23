#include "VanityCamera.h"

namespace VanityCamera {
void Disable(const char* a_source) {
    if (auto* playerCamera = RE::PlayerCamera::GetSingleton()) {
        auto& cameraData = playerCamera->GetRuntimeData2();
        cameraData.allowAutoVanityMode = false;
        cameraData.idleTimer = 0.0f;
    } else {
        logger::warn("VanityCamera: disable skipped | reason=noPlayerCamera | source={}", a_source);
    }
}
}
