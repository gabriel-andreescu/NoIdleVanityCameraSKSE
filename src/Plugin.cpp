#include "Hooks.h"

#include <SKSE/SKSE.h>

#include <REL/Module.h>

namespace {
constexpr auto kTrampolineSize = 64UZ;

void MessageHandler(SKSE::MessagingInterface::Message* a_message) { // NOLINT(misc-const-correctness)
    if (a_message->type == SKSE::MessagingInterface::kDataLoaded) {
        Hooks::Install();
    }
}
}

SKSE_PLUGIN_LOAD(const SKSE::LoadInterface* a_extender) {
    if (REL::Module::IsVR()) {
        return false;
    }

    SKSE::Init(
        a_extender,
        {
            .logPattern = "[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%t] [%s:%#] %v",
            .trampoline = true,
            .trampolineSize = kTrampolineSize,
        }
    );

    SKSE::GetMessagingInterface()->RegisterListener(MessageHandler);

    return true;
}
