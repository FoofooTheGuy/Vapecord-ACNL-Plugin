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
			u32 GetPlayerMessageData(void);
			std::string GetPlayerMessage(void);
			void ClearPlayerMessage(void);
			std::string GetPlayerName(void);
			u8 GetPlayerIndex(void);
			bool IsPlayerMessageOnScreen(void);
            static void EnableCommands(void);
            static void DisableCommands(void);

        protected:
            static void CommandCallback(void);

        private:    
            void CommandLoop(void);
			void AnimationCommand(void);
			void EmotionCommand(void);
			void SnakeCommand(void);
			void MusicCommand(void);
			void ItemCommand(void);

			u8 animID = 6;
			u8 emotionID = 1;
			u16 snakeID = 1;
			u16 musicID = 0x660;
			CTRPluginFramework::Item itemID = { 0x7FFE, 0 };
	};
}