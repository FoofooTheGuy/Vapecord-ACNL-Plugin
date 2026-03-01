#pragma once

#include <CTRPluginFramework.hpp>
#include "core/game_api/GameStructs.hpp"

namespace CTRPluginFramework {
	namespace GameKeyboard {
		bool 				Write(const std::string& str);
		bool 				Delete(void);
		bool		 		CopySelected(std::string& str);
		bool 				DeleteSelected(void);
		bool 				IsOpen();
		bool				IsEmpty();
		bool 				Copy(std::string& str, int pos, int length);
		bool 				ConvertToItemID(std::string& str, Item &ItemID);
		void 				SendMessage(const std::string& str);
	}
}