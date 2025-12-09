#include "PCH.h"

namespace {
void DisableAutoVanity() {
    if (auto* playerCamera = RE::PlayerCamera::GetSingleton()) {
        playerCamera->GetRuntimeData2().allowAutoVanityMode = false;
        logger::info("Disabled auto vanity camera mode");
    } else {
        logger::warn("PlayerCamera unavailable");
    }
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void MessageHandler(SKSE::MessagingInterface::Message* a_msg) {
    if (a_msg->type
        == SKSE::MessagingInterface::kDataLoaded
        || a_msg->type
        == SKSE::MessagingInterface::kNewGame
        || a_msg->type
        == SKSE::MessagingInterface::kPostLoadGame) {
        DisableAutoVanity();
    }
}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
    std::shared_ptr<spdlog::sinks::sink> sink;
    if (IsDebuggerPresent()) {
        sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
    } else {
        // ReSharper disable once CppLocalVariableMayBeConst
        auto path = SKSE::log::log_directory();
        if (!path) {
            stl::report_and_fail("Failed to find standard logging directory"sv);
        }
        const auto* plugin = SKSE::PluginDeclaration::GetSingleton();
        *path /= std::format("{}.log", plugin->GetName());
        sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
    }

    auto logger = std::make_shared<spdlog::logger>("Global", std::move(sink));
    logger->set_level(
#ifdef NDEBUG
        spdlog::level::info
#else
        spdlog::level::debug
#endif
    );
    logger->flush_on(spdlog::level::trace);
    spdlog::set_default_logger(std::move(logger));
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%t] [%s:%#] %v");

    SKSE::Init(a_skse, false);

    const auto* msg = SKSE::GetMessagingInterface();
    if (!msg) {
        stl::report_and_fail("Failed to get SKSE messaging interface.");
    }

    if (!msg->RegisterListener(MessageHandler)) {
        stl::report_and_fail("Failed to register for SKSE messages.");
    }

    return true;
}
