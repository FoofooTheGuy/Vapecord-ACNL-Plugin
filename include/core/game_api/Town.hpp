#pragma once

#include <CTRPluginFramework.hpp>
#include <core/game_api/GameStructs.hpp>

namespace CTRPluginFramework {
	namespace Town {
		ACNL_TownData *GetSaveData(void);
		void 		   EditName(const std::string& name);
	}
}