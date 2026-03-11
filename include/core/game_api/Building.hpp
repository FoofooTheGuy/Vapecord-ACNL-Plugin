#pragma once

#include <CTRPluginFramework.hpp>
#include "core/game_api/GameStructs.hpp"
#include "core/game_api/PlayerClass.hpp"

namespace CTRPluginFramework {
    enum class EditBuildingResult {
		Success,
        BuildingDataNotLoaded,
		NoPlayerLoaded,
		NotInTown,
		InvalidBuildingId,
		NotInOwnTown,
		NoFreeSlot,
		NoFreeDesignStand,
		CannotRemoveBuilding
	};

	namespace Building {
		ACNL_BuildingData 	    *GetSaveData();

		EditBuildingResult     TryPlaceBuilding(u8 buildingID);
		EditBuildingResult     TryMoveBuilding(u8 buildingSlotIndex);
		EditBuildingResult     TryRemoveBuilding(u8 buildingSlotIndex);
	};
}