#pragma once

#include <CTRPluginFramework.hpp>
#include "core/game_api/Item.hpp"
#include "core/game_api/Game.hpp"
#include "core/infrastructure/Address.hpp"
#include "core/game_api/Animation.hpp"
#include "core/game_api/Dropper.hpp"
#include "core/checks/IDChecks.hpp"
#include "platform/ctrpf/UtilsExtras.hpp"

namespace CTRPluginFramework {
	class Chat {
	public:
		static void EnableCommands(void);
		static void DisableCommands(void);

	private:
		static void CommandCallback(void);
		static void ProcessChatCommand(void);
	};
}