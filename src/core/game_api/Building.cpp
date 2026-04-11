#include "core/game_api/Building.hpp"
#include "core/game_api/Game.hpp"
#include "core/game_api/PlayerClass.hpp"
#include "core/checks/IDChecks.hpp"
#include "core/infrastructure/Address.hpp"

namespace CTRPluginFramework {
	ACNL_BuildingData *Building::GetSaveData() {
		u32 *addr = (u32 *)(Address(0x2FB344).Call<u32>() + 0x4BE80);
		return (ACNL_BuildingData *)addr;
	}

	namespace {
		constexpr u8 kEmptyBuildingId = 0xFC;

		bool IsFaceCutOutBuilding(u8 buildingID) {
			return buildingID == 0xDC || buildingID == 0xDD;
		}

		void PlaceBuildingUpdateCollisions(u32 x, u32 y, u16 buildingID) {
			static Address placeBuilding(0x2425D8);
			placeBuilding.Call<void>(x, y, buildingID);
		}

		void EditFaceCutOutData(ACNL_BuildingData *building, u32 oldX, u32 oldY, u32 newX, u32 newY) {
			for(int i = 0; i < 8; ++i) {
				if(building->Stands[i].xCoord == oldX && building->Stands[i].yCoord == oldY) {
					building->Stands[i].xCoord = newX & 0xFF;
					building->Stands[i].yCoord = newY & 0xFF;
					return;
				}
			}
		}

		bool IsBuildingSpotFree(ACNL_BuildingData *building) {
			for(int slot = 0; slot < 56; ++slot) {
				if(building->Buildings.Building[slot].ID == kEmptyBuildingId) {
					return true;
				}
			}

			return false;
		}

		bool IsFaceCutOutSpaceFree(ACNL_BuildingData *building) {
			for(int i = 0; i < 8; ++i) {
				if(building->Stands[i].xCoord == -1 && building->Stands[i].yCoord == -1) {
					return true;
				}
			}

			return false;
		}

		void SetFaceCutOutData(ACNL_BuildingData *building, u32 x, u32 y) {
			for(int i = 0; i < 8; ++i) {
				if(building->Stands[i].xCoord == -1 && building->Stands[i].yCoord == -1) {
					building->Stands[i].xCoord = x & 0xFF;
					building->Stands[i].yCoord = y & 0xFF;

					for(int j = 0; j < 8; ++j) {
						if(building->Stands[j].xCoord == -1 && building->Stands[j].yCoord == -1) {
							building->Stands[j].Pattern = building->Stands[i].Pattern;
						}
					}

					return;
				}
			}
		}

		EditBuildingResult ValidateBuildingContext(ACNL_BuildingData *building) {
			if(!building) {
				return EditBuildingResult::BuildingDataNotLoaded;
			}

			if(!PlayerClass::GetInstance()->IsLoaded()) {
				return EditBuildingResult::NoPlayerLoaded;
			}

			if(!Game::IsGameInRoom(0)) {
				return EditBuildingResult::NotInTown;
			}

			if(Game::GetOnlinePlayerCount() != 0) {
				return EditBuildingResult::NotInOwnTown;
			}

			return EditBuildingResult::Success;
		}

		EditBuildingResult ValidatePlaceBuilding(ACNL_BuildingData *building, u8 buildingID) {
			EditBuildingResult context = ValidateBuildingContext(building);
			if(context != EditBuildingResult::Success) {
				return context;
			}

			if(!IDChecks::BuildingValid(buildingID)) {
				return EditBuildingResult::InvalidBuildingId;
			}

			if(!IsBuildingSpotFree(building)) {
				return EditBuildingResult::NoFreeSlot;
			}

			if(IsFaceCutOutBuilding(buildingID) && !IsFaceCutOutSpaceFree(building)) {
				return EditBuildingResult::NoFreeDesignStand;
			}

			return EditBuildingResult::Success;
		}

		EditBuildingResult ValidateSelectedBuilding(ACNL_BuildingData *building, u8 buildingSlotIndex) {
			EditBuildingResult context = ValidateBuildingContext(building);
			if(context != EditBuildingResult::Success) {
				return context;
			}

			if(buildingSlotIndex >= 56 || building->Buildings.Building[buildingSlotIndex].ID == kEmptyBuildingId) {
				return EditBuildingResult::InvalidBuildingId;
			}

			return EditBuildingResult::Success;
		}

		void ReloadAfterBuildingEdit(bool movePlayerForward) {
			Sleep(Milliseconds(20));

			if(movePlayerForward) {
				u32 x, y;
				PlayerClass::GetInstance()->GetWorldCoords(&x, &y);
				y += 2;

				float *coords = PlayerClass::GetInstance()->GetCoordinates(x, y);
				if(coords) {
					Game::ReloadRoom(coords);
				}
				return;
			}

			Game::ReloadRoom();
		}
	}

	EditBuildingResult Building::TryPlaceBuilding(u8 buildingID) {
		ACNL_BuildingData *building = GetSaveData();

		EditBuildingResult validation = ValidatePlaceBuilding(building, buildingID);
		if(validation != EditBuildingResult::Success) {
			return validation;
		}

		u32 x, y;
		PlayerClass::GetInstance()->GetWorldCoords(&x, &y);
		PlaceBuildingUpdateCollisions(x, y, buildingID);

		if(IsFaceCutOutBuilding(buildingID)) {
			SetFaceCutOutData(building, x, y);
		}

		ReloadAfterBuildingEdit(true);
		return EditBuildingResult::Success;
	}

	EditBuildingResult Building::TryMoveBuilding(u8 buildingSlotIndex) {
		ACNL_BuildingData *building = GetSaveData();

		EditBuildingResult validation = ValidateSelectedBuilding(building, buildingSlotIndex);
		if(validation != EditBuildingResult::Success) {
			return validation;
		}

		u32 x, y;
		PlayerClass::GetInstance()->GetWorldCoords(&x, &y);

		ACNL_Building &selectedBuilding = building->Buildings.Building[buildingSlotIndex];
		if(IsFaceCutOutBuilding(selectedBuilding.ID)) {
			EditFaceCutOutData(building, selectedBuilding.XCoord, selectedBuilding.YCoord, x, y);
		}

		selectedBuilding.XCoord = x & 0xFF;
		selectedBuilding.YCoord = y & 0xFF;

		ReloadAfterBuildingEdit(false);
		return EditBuildingResult::Success;
	}

	EditBuildingResult Building::TryRemoveBuilding(u8 buildingSlotIndex) {
		ACNL_BuildingData *building = GetSaveData();

		EditBuildingResult validation = ValidateSelectedBuilding(building, buildingSlotIndex);
		if(validation != EditBuildingResult::Success) {
			return validation;
		}

		ACNL_Building &selectedBuilding = building->Buildings.Building[buildingSlotIndex];
		if(!IDChecks::BuildingValid(selectedBuilding.ID)) {
			return EditBuildingResult::CannotRemoveBuilding;
		}

		if(IsFaceCutOutBuilding(selectedBuilding.ID)) {
			for(int i = 0; i < 8; ++i) {
				if(building->Stands[i].xCoord == selectedBuilding.XCoord && building->Stands[i].yCoord == selectedBuilding.YCoord) {
					building->Stands[i].xCoord = -1;
					building->Stands[i].yCoord = -1;
				}
			}
		}

		selectedBuilding.ID = kEmptyBuildingId;
		selectedBuilding.XCoord = 0;
		selectedBuilding.YCoord = 0;
		building->NormalPWPsAmount--;

		ReloadAfterBuildingEdit(false);
		return EditBuildingResult::Success;
	}
}