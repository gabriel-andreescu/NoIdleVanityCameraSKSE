#include "Hooks.h"
#include "PCH.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

#include <format>
#include <memory>
#include <utility>

namespace {
constexpr auto kTrampolineSize = 64;

void MessageHandler(SKSE::MessagingInterface::Message* a_msg) {
    if (!a_msg) {
        return;
    }

    switch (a_msg->type) {
        case SKSE::MessagingInterface::kDataLoaded: Hooks::Install(); break;
        default:                                    break;
    }
}

void InitializeLogging() {
    std::shared_ptr<spdlog::sinks::sink> sink;
    if (IsDebuggerPresent()) {
        sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
    } else {
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
}
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
    InitializeLogging();

    SKSE::Init(a_skse, false);
    SKSE::AllocTrampoline(kTrampolineSize);

    const auto* messaging = SKSE::GetMessagingInterface();
    if (!messaging) {
        logger::critical("SKSE: messaging interface unavailable");
        return false;
    }

    messaging->RegisterListener(MessageHandler);
    logger::info("NoIdleVanityCameraSKSE loaded");
    return true;
}
