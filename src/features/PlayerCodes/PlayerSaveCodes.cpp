#include "features/cheats.hpp"
#include "core/game_api/PlayerClass.hpp"
#include "core/game_api/Player.hpp"
#include "core/checks/IDChecks.hpp"
#include "core/infrastructure/PluginUtils.hpp"
#include "core/game_api/Game.hpp"
#include "core/infrastructure/Address.hpp"
#include "core/game_api/GameStructs.hpp"
#include "core/Converters.hpp"
#include "core/game_api/Town.hpp"

#include "Color.h"
#include "Files.h"

namespace CTRPluginFramework {
	void NameChanger(MenuEntry* entry) {
		Language *language = Language::getInstance();
		if(!Player::GetSaveData()) {
			MessageBox(language->get(TextID::SAVE_PLAYER_NO)).SetClear(ClearScreen::Top)();
			return;
		}

		Keyboard keyboard(language->get(TextID::NAME_CHANGER_ENTER_NAME));
		std::string input = "";
		keyboard.SetMaxLength(8);

		if(keyboard.Open(input) < 0) {
			return;
		}

		Player::EditName(4, input);
		MessageBox(Utils::Format(language->get(TextID::NAME_CHANGER_SET).c_str(), input.c_str())).SetClear(ClearScreen::Top)();
	}

	void playermod(MenuEntry *entry) {
		ACNL_Player *player = Player::GetSaveData();
		Language *language = Language::getInstance();

		if(!player) {
			MessageBox(language->get(TextID::SAVE_PLAYER_NO)).SetClear(ClearScreen::Both)();
			return;
		}

		static const u8 ValidAppearanceID[4][2] = {
			{ 0x00, 0x21 }, { 0x00, 0x0F },
			{ 0x00, 0x0B }, { 0x00, 0x05 }
		};

		static const u16 ValidOutfitID[6][2] = {
			{ 0x280B, 0x28F3 },
			{ 0x28F5, 0x295B },
			{ 0x2493, 0x26F5 },
			{ 0x26F8, 0x2776 },
			{ 0x2777, 0x279E },
			{ 0x279F, 0x27E5 }
		};

		const std::vector<std::string> playeropt = {
			language->get(TextID::VECTOR_PLAYER_MOD_HAIR_STYLE),
			language->get(TextID::VECTOR_PLAYER_MOD_HAIR_COLOR),
			language->get(TextID::VECTOR_PLAYER_MOD_EYE_STYLE),
			language->get(TextID::VECTOR_PLAYER_MOD_EYE_COLOR),

			language->get(TextID::VECTOR_PLAYER_MOD_GENDER),
			language->get(TextID::VECTOR_PLAYER_MOD_TAN),
			language->get(TextID::VECTOR_PLAYER_MOD_OUTFIT)
		};

		const std::vector<std::string> genderopt = {
			language->get(TextID::VECTOR_PLAYER_MOD_GENDER_MALE),
			language->get(TextID::VECTOR_PLAYER_MOD_GENDER_FEMALE),
		};

		const std::vector<std::string> tanopt = {
			language->get(TextID::VECTOR_PLAYER_MOD_TAN_DARK),
			language->get(TextID::VECTOR_PLAYER_MOD_TAN_TAN),
			language->get(TextID::VECTOR_PLAYER_MOD_TAN_FAIR),
			language->get(TextID::VECTOR_PLAYER_MOD_TAN_CUSTOM),
		};

		const std::vector<std::string> outfitplayeropt = {
			language->get(TextID::VECTOR_OUTFIT_HEADGEAR),
			language->get(TextID::VECTOR_OUTFIT_GLASSES),
			language->get(TextID::VECTOR_OUTFIT_SHIRT),
			language->get(TextID::VECTOR_OUTFIT_PANTS),
			language->get(TextID::VECTOR_OUTFIT_SOCKS),
			language->get(TextID::VECTOR_OUTFIT_SHOES)
		};

		u8 ID = 0;
		u16 item = 0;
		bool updateStyle = false;
		bool updateTan = false;

		Keyboard optKb(language->get(TextID::KEY_PLAYER_MOD_CHOOSE_ACTION), playeropt);

		int choice = optKb.Open();
		if(choice < 0) {
			return;
		}

		switch(choice) {
			case 0:
			case 1:
			case 2:
			case 3:
				KeyRange::Set({ ValidAppearanceID[choice][0], ValidAppearanceID[choice][1] });
				if(PluginUtils::Input::PromptNumber<u8>({ language->get(TextID::ENTER_ID) << Utils::Format("%02X -> %02X", ValidAppearanceID[choice][0], ValidAppearanceID[choice][1]), true, 2, ID, ValidKeyboardCheck }, ID)) {
					switch(choice) {
						case 0: player->PlayerAppearance.PlayerFeatures.HairStyle = ID; break;
						case 1: player->PlayerAppearance.PlayerFeatures.HairColor = ID; break;
						case 2: player->PlayerAppearance.PlayerFeatures.Face = ID; break;
						case 3: player->PlayerAppearance.PlayerFeatures.EyeColor = ID; break;
					}
					updateStyle = true;
				}
			break;

			case 4: {
				optKb.Populate(genderopt);
				int gender = optKb.Open();
				if(gender < 0) {
					return;
				}

				Player::EditGender(4, gender);
			} break;

			case 5: {
				optKb.Populate(tanopt);
				int tanChoice = optKb.Open();
				if(tanChoice < 0) {
					return;
				}

				switch(tanChoice) {
					case 0:
						player->PlayerAppearance.PlayerFeatures.Tan = 0xF;
						updateTan = true;
						break;
					case 1:
						player->PlayerAppearance.PlayerFeatures.Tan = 0xA;
						updateTan = true;
						break;
					case 2:
						player->PlayerAppearance.PlayerFeatures.Tan = 0;
						updateTan = true;
						break;
					case 3: {
						u8 val = 0;
						if(PluginUtils::Input::PromptNumber<u8>({ language->get(TextID::PLAYER_APPEARANCE_TAN_LEVEL) << "0x00 -> 0x0F", false, 2, 0 }, val)) {
							player->PlayerAppearance.PlayerFeatures.Tan = val;
							updateTan = true;
						}
					} break;
					default:
						break;
				}
			} break;

			case 6: {
				optKb.Populate(outfitplayeropt);
				int res = optKb.Open();
				if(res < 0) {
					return;
				}

				KeyRange::Set({ ValidOutfitID[res][0], ValidOutfitID[res][1] });
				if(PluginUtils::Input::PromptNumber<u16>({ language->get(TextID::ENTER_ID) << Utils::Format("%04X -> %04X", ValidOutfitID[res][0], ValidOutfitID[res][1]), true, 4, item, ValidKeyboardCheck }, item)) {
					switch(res) {
						case 0: player->PlayerAppearance.PlayerOutfit.Hat.ID = item; break;
						case 1: player->PlayerAppearance.PlayerOutfit.Accessory.ID = item; break;
						case 2: player->PlayerAppearance.PlayerOutfit.TopWear.ID = item; break;
						case 3: player->PlayerAppearance.PlayerOutfit.BottomWear.ID = item; break;
						case 4: player->PlayerAppearance.PlayerOutfit.Socks.ID = item; break;
						case 5: player->PlayerAppearance.PlayerOutfit.Shoes.ID = item; break;
					}

					Player::WriteOutfit(Game::GetOnlinePlayerIndex(),
						player->PlayerAppearance.PlayerOutfit.Hat,
						player->PlayerAppearance.PlayerOutfit.Accessory,
						player->PlayerAppearance.PlayerOutfit.TopWear,
						player->PlayerAppearance.PlayerOutfit.BottomWear,
						player->PlayerAppearance.PlayerOutfit.Socks,
						player->PlayerAppearance.PlayerOutfit.Shoes);
					updateStyle = true;
				}
			} break;

			default:
				break;
		}

		if(updateTan) {
			Player::UpdateTan();
		}

		if(updateStyle) {
			Player::UpdateStyle();
		}
	}

	void randomoutfit(MenuEntry *entry) {
		ACNL_Player *player = Player::GetSaveData();
		Language *language = Language::getInstance();

		if(!player) {
			MessageBox(language->get(TextID::SAVE_PLAYER_NO)).SetClear(ClearScreen::Both)();
			return;
		}

		const std::vector<std::string> randomopt = {
			language->get(TextID::VECTOR_RANDOM_OUTFIT),
			language->get(TextID::VECTOR_RANDOM_PLAYER)
		};

		Keyboard randkb(language->get(TextID::KEY_RANDOMIZE_PLAYER), randomopt);
		switch(randkb.Open()) {
			default: break;
			case 0:
				if(!(MessageBox(language->get(TextID::RANDOM_OUTFIT_WARNING), DialogType::DialogYesNo)).SetClear(ClearScreen::Both)()) {
					return;
				}

				Player::WriteOutfit(Game::GetOnlinePlayerIndex(), (Item)Utils::Random(0x280B, 0x28F3),
																		(Item)Utils::Random(0x28F5, 0x295B),
																		(Item)Utils::Random(0x2493, 0x26F5),
																		(Item)Utils::Random(0x26F8, 0x2776),
																		(Item)Utils::Random(0x2777, 0x279E),
																		(Item)Utils::Random(0x279F, 0x27E5));
			break;
			case 1: {
				if(!(MessageBox(language->get(TextID::RANDOM_PLAYER_WARNING), DialogType::DialogYesNo)).SetClear(ClearScreen::Both)()) {
					return;
				}

				player->PlayerAppearance.PlayerFeatures.HairStyle = Utils::Random(0, 0x21);
				player->PlayerAppearance.PlayerFeatures.HairColor = Utils::Random(0, 0xF);
				player->PlayerAppearance.PlayerFeatures.Face = Utils::Random(0, 4);
				player->PlayerAppearance.PlayerFeatures.EyeColor = Utils::Random(0, 4);
				player->PlayerAppearance.PlayerFeatures.Tan = Utils::Random(0, 0xF);

				player->PlayerAppearance.PlayerOutfit.Hat.ID = Utils::Random(0x280B, 0x28F3);
				player->PlayerAppearance.PlayerOutfit.Accessory.ID = Utils::Random(0x28F5, 0x295B);
				player->PlayerAppearance.PlayerOutfit.TopWear.ID = Utils::Random(0x2493, 0x26F5);
				player->PlayerAppearance.PlayerOutfit.BottomWear.ID = Utils::Random(0x26F8, 0x2776);
				player->PlayerAppearance.PlayerOutfit.Socks.ID = Utils::Random(0x2777, 0x279E);
				player->PlayerAppearance.PlayerOutfit.Shoes.ID = Utils::Random(0x279F, 0x27E5);

				Player::UpdateTan();
				Player::UpdateStyle();
			} break;
		}
	}

	void playerbackup(MenuEntry *entry) {
		ACNL_Player *player = Player::GetSaveData();
		Language *language = Language::getInstance();
		if(!player) {
			MessageBox(language->get(TextID::SAVE_PLAYER_NO)).SetClear(ClearScreen::Both)();
			return;
		}

		MemoryRange locPlayer = { (u32 *)player, sizeof(ACNL_Player) };

		const std::vector<std::string> backopt = {
			language->get(TextID::VECTOR_RANDOM_BACKUP),
			language->get(TextID::VECTOR_RANDOM_RESTORE),
			language->get(TextID::FILE_DELETE),
		};

		Keyboard backkb(language->get(TextID::KEY_PLAYER_BACKUP_CHOOSE_ACTION), backopt);
		switch(backkb.Open()) {
			default: break;
			case 0: {
				std::string filename = "";
				Keyboard KB(language->get(TextID::RANDOM_PLAYER_DUMP));

				if(KB.Open(filename) == -1) {
					return;
				}

				PluginUtils::Backup::DumpMemory(
					Utils::Format(PATH_PLAYER, Address::regionName.c_str()),
					filename,
					".player",
					{ locPlayer }
				);
			} break;
			case 1: {
				PluginUtils::Backup::RestoreMemory(
					Utils::Format(PATH_PLAYER, Address::regionName.c_str()),
					".player",
					language->get(TextID::RANDOM_PLAYER_RESTORE),
					{ locPlayer }
				);
				Player::UpdateTan();
				Player::UpdateStyle();
			} break;
			case 2:
				PluginUtils::Backup::DeleteBackup(
					Utils::Format(PATH_PLAYER, Address::regionName.c_str()),
					".player"
				);
			break;
		}
	}

	void tpcmessage(MenuEntry* entry) {
		ACNL_Player *player = Player::GetSaveData();
		Language *language = Language::getInstance();
		if(!player) {
			MessageBox(language->get(TextID::SAVE_PLAYER_NO)).SetClear(ClearScreen::Top)();
			return;
		}

		std::string input = "";

		Keyboard KB(language->get(TextID::TPC_MESSAGE_ENTER_NAME));
		KB.SetMaxLength(26);

		if(KB.Open(input) >= 0) {
			Convert::STR_TO_U16(input, player->TPCText);
			MessageBox(Utils::Format(language->get(TextID::TPC_MESSAGE_SET).c_str(), input.c_str())).SetClear(ClearScreen::Top)();
		}
	}

	void tpc(MenuEntry *entry) {
		ACNL_Player *player = Player::GetSaveData();
		Language *language = Language::getInstance();
		if(!player) {
			MessageBox(language->get(TextID::SAVE_PLAYER_NO)).SetClear(ClearScreen::Top)();
			return;
		}

		std::vector<std::string> playerOptions = {
			Color::Silver << language->get(TextID::SAVE_PLAYER_EMPTY),
			Color::Silver << language->get(TextID::SAVE_PLAYER_EMPTY),
			Color::Silver << language->get(TextID::SAVE_PLAYER_EMPTY),
			Color::Silver << language->get(TextID::SAVE_PLAYER_EMPTY),
		};

		std::vector<bool> validPlayerOption = { false, false, false, false };

		for(int i = 0; i <= 3; ++i) {
			ACNL_Player *loadedPlayer = Player::GetSaveData(i);
			if(loadedPlayer && Player::SaveExists(loadedPlayer)) {
				std::string playerName = "";
				Convert::U16_TO_STR(loadedPlayer->PlayerInfo.PlayerData.PlayerName, playerName);
				playerOptions[i] = Player::GetColor(i) << playerName;
				validPlayerOption[i] = true;
			}
		}

		const std::vector<std::string> tpcselectopt = {
			language->get(TextID::VECTOR_TPCDUMP_DUMP),
			language->get(TextID::VECTOR_TPCDUMP_RESTORE),
			language->get(TextID::FILE_DELETE),
		};

		MemoryRange locTPC;

		Keyboard KB(language->get(TextID::KEY_TPC_CHOOSE_ACTION), tpcselectopt);

		switch(KB.Open()) {
			default: break;

			case 0: {
				Keyboard PKB(language->get(TextID::KEY_TPC_SELECT_BACKUP_PLAYER), playerOptions);

				int index = PKB.Open();
				if(index < 0) {
					return;
				}

				if(!validPlayerOption[index]) {
					MessageBox(language->get(TextID::PLAYER_SELECT_PLAYER_NOT_EXISTS)).SetClear(ClearScreen::Top)();
					return;
				}

				player = Player::GetSaveData(index);
				if(player) {
					std::string filename = "";
					Keyboard KB(language->get(TextID::TPC_DUMPER_NAME));

					if(KB.Open(filename) < 0) {
						return;
					}

					locTPC = { (u32 *)player->TPCPic, sizeof(player->TPCPic) };
					PluginUtils::Backup::DumpMemory(
						Utils::Format(PATH_TPC, Address::regionName.c_str()),
						filename,
						".jpg",
						{ locTPC }
					);
				}
			} break;

			case 1:
				player = Player::GetSaveData();
				locTPC = { (u32 *)player->TPCPic, sizeof(player->TPCPic) };
				PluginUtils::Backup::RestoreMemory(
					Utils::Format(PATH_TPC, Address::regionName.c_str()),
					".jpg",
					language->get(TextID::TPC_DUMPER_RESTORE_CURRENT),
					{ locTPC }
				);
			break;

			case 2:
				PluginUtils::Backup::DeleteBackup(
					Utils::Format(PATH_TPC, Address::regionName.c_str()),
					".jpg"
				);
			break;
		}
	}

//dump designs | player specific save code
	void DesignDumper(MenuEntry *entry) {
		ACNL_Player *player = Player::GetSaveData();
		if(!player) {
			MessageBox(Language::getInstance()->get(TextID::SAVE_PLAYER_NO)).SetClear(ClearScreen::Top)();
			return;
		}

		std::vector<std::string> designslots;

		for(int i = 1; i <= 10; ++i) {
			designslots.push_back(Utils::Format(Language::getInstance()->get(TextID::VECTOR_DESIGN).c_str(), i));
		}

		const std::vector<std::string> designselect = {
			Language::getInstance()->get(TextID::VECTOR_DESIGNDUMP_DUMP),
			Language::getInstance()->get(TextID::VECTOR_DESIGNDUMP_RESTORE),
			Language::getInstance()->get(TextID::FILE_DELETE),
		};

		MemoryRange locPattern;
		int dSlot = 0;

		Keyboard KB(Language::getInstance()->get(TextID::KEY_DESIGN_DUMP_CHOOSE_ACTION), designselect);

		switch(KB.Open()) {
			default: break;

			case 0: {
				Keyboard DKB(Language::getInstance()->get(TextID::KEYBOARD_DESIGNDUMP), designslots);

				dSlot = DKB.Open();
				if(dSlot < 0) {
					return;
				}

				std::string filename = "";
				Keyboard KB(Language::getInstance()->get(TextID::DESIGN_DUMP_NAME));

				if(KB.Open(filename) < 0) {
					return;
				}

				locPattern = { (u32 *)&player->Patterns[player->PatternOrder[dSlot]], sizeof(ACNL_Pattern) };
				PluginUtils::Backup::DumpMemory(
					Utils::Format(PATH_DESIGN, Address::regionName.c_str()),
					filename,
					".acnl",
					{ locPattern }
				);
			} break;

			case 1: {
				Keyboard DKB(Language::getInstance()->get(TextID::KEYBOARD_DESIGNDUMP), designslots);

				dSlot = DKB.Open();
				if(dSlot < 0) {
					return;
				}

				locPattern = { (u32 *)&player->Patterns[player->PatternOrder[dSlot]], sizeof(ACNL_Pattern) };
				PluginUtils::Backup::RestoreMemory(
					Utils::Format(PATH_DESIGN, Address::regionName.c_str()),
					".acnl",
					Language::getInstance()->get(TextID::DESIGN_DUMP_RESTORE),
					{ locPattern }
				);
				Player::ReloadDesign(player->PatternOrder[dSlot]);
			} break;

			case 2:
				PluginUtils::Backup::DeleteBackup(
					Utils::Format(PATH_DESIGN, Address::regionName.c_str()),
					".acnl"
				);
			break;
		}
	}
//Fill Emote List | player specific save code
	void emotelist(MenuEntry *entry) {
		ACNL_Player *player = Player::GetSaveData();
		Language *language = Language::getInstance();

		if(!player) {
			MessageBox(language->get(TextID::SAVE_PLAYER_NO)).SetClear(ClearScreen::Top)();
			return;
		}

		const std::vector<std::string> emoteopt = {
			language->get(TextID::VECTOR_EMOTIONLIST_FILL_LIST),
			language->get(TextID::VECTOR_EMOTIONLIST_FILL_EMOTION),
			language->get(TextID::VECTOR_EMOTIONLIST_CLEAR_LIST),
		};

		static Address emoticons(0x8902A4);
		Emoticons *gameEmotes = new Emoticons();
		gameEmotes = (Emoticons *)emoticons.addr;
		if(!gameEmotes) {
			return;
		}

		Keyboard KB(language->get(TextID::KEY_EMOTION_LIST_CHOOSE_ACTION), emoteopt);

		switch(KB.Open()) {
			default: break;
			case 0:
				player->Emotes = *gameEmotes;
				MessageBox(language->get(TextID::EMOTION_LIST_FILL_SUCCESS)).SetClear(ClearScreen::Top)();
			break;
			case 1: {
				u8 emotion = 0;
				Keyboard KB(language->get(TextID::EMOTION_LIST_TYPE_ID));
				KB.IsHexadecimal(true);

				if(KB.Open(emotion) < 0) {
					return;
				}

				std::memset((void *)player->Emotes.emoticons, emotion, 0x28);
				MessageBox(Utils::Format(language->get(TextID::EMOTION_LIST_SET_SUCCESS).c_str(), emotion)).SetClear(ClearScreen::Top)();
			} break;

			case 2:
				std::memset((void *)player->Emotes.emoticons, 0, 0x28);
				MessageBox(language->get(TextID::EMOTION_LIST_CLEAR_SUCCESS)).SetClear(ClearScreen::Top)();
			break;
		}
	}

	void enzyklopedia(MenuEntry *entry) {
		ACNL_Player *player = Player::GetSaveData();
		Language *language = Language::getInstance();

		if(!player) {
			MessageBox(language->get(TextID::SAVE_PLAYER_NO)).SetClear(ClearScreen::Top)();
			return;
		}

		const std::vector<std::string> enzyopt = {
			language->get(TextID::VECTOR_ENZY_FILL),
			language->get(TextID::VECTOR_ENZY_CLEAR),
		};

		static const Item_Category EncyclopediaID[3] = {
			Item_Category::Bugs, Item_Category::Fish,
			Item_Category::SeaCreatures
		};

		Keyboard KB(language->get(TextID::KEY_ENZYKLOPEDIA_CHOOSE_ACTION), enzyopt);

		switch(KB.Open()) {
			default: break;
			case 0:
				for(int i = 0; i < 3; ++i) {
					Player::SetUnlockableBitField(player, EncyclopediaID[i], true);
				}

				for(int i = 0; i < 72; ++i) {
					player->EncyclopediaSizes.Insects[i] = Utils::Random(1, 0x3FFF);
					player->EncyclopediaSizes.Fish[i] = Utils::Random(1, 0x3FFF);

					if(i < 30) {
						player->EncyclopediaSizes.SeaCreatures[i] = Utils::Random(1, 0x3FFF);
					}
				}
				MessageBox(language->get(TextID::ENZYKLOPEDIA_FILL_SUCCESS)).SetClear(ClearScreen::Top)();
			break;
			case 1:
				for(int i = 0; i < 3; ++i) {
					Player::SetUnlockableBitField(player, EncyclopediaID[i], false);
				}

				for(int i = 0; i < 72; ++i) {
					player->EncyclopediaSizes.Insects[i] = 0;
					player->EncyclopediaSizes.Fish[i] = 0;

					if(i < 30) {
						player->EncyclopediaSizes.SeaCreatures[i] = 0;
					}
				}
				MessageBox(language->get(TextID::ENZYKLOPEDIA_CLEAR_SUCCESS)).SetClear(ClearScreen::Top)();
			break;
		}
	}

	void comodifier(MenuEntry *entry) {
		ACNL_Player *player = Player::GetSaveData();
		Language *language = Language::getInstance();

		if(!player) {
			MessageBox(language->get(TextID::SAVE_PLAYER_NO)).SetClear(ClearScreen::Top)();
			return;
		}

		Keyboard kb(language->get(TextID::DREAM_CODE_ENTER_ID));
		kb.IsHexadecimal(true);
		kb.DisplayTopScreen = true;

		u16 part1, part2, part3;

		if(kb.Open(part1, 0) >= 0) {
			if(kb.Open(part2, 0) >= 0) {
				if(kb.Open(part3, 0) >= 0) {
					player->DreamCode.DCPart1 = (part2 << 16) + part3;
					player->DreamCode.DCPart2 = (part1 & 0xFF);
					player->DreamCode.DCPart3 = (part1 >> 8);

					player->DreamCode.HasDreamAddress = true;
					MessageBox(Utils::Format(language->get(TextID::DREAM_CODE_SET).c_str(), part1, part2, part3)).SetClear(ClearScreen::Top)();
				}
			}
		}
	}
//Enable Unused Menu | player specific save code
	void debug1(MenuEntry *entry) {
		static Address currentSelectedTPCMenu(0x951014);

		ACNL_Player *player = Player::GetSaveData();

		if(!player) {
			MessageBox(Language::getInstance()->get(TextID::SAVE_PLAYER_NO)).SetClear(ClearScreen::Top)();
			return;
		}

		std::vector<std::string> cmnOpt =  { "" };

		bool IsON = player->PlayerFlags.CanUseCensusMenu == 1;

		cmnOpt[0] = IsON ? (Color(pGreen) << Language::getInstance()->get(TextID::VECTOR_ENABLED)) : (Color(pRed) << Language::getInstance()->get(TextID::VECTOR_DISABLED));

		Keyboard KB(Language::getInstance()->get(TextID::KEY_CHOOSE_OPTION), cmnOpt);

		int op = KB.Open();
		if(op < 0) {
			return;
		}

		if (IsON) {
			player->PlayerFlags.CanUseCensusMenu = false;
			if (*(u8 *)(currentSelectedTPCMenu.addr) == 1) { //Census Menu is selected
				*(u8 *)(currentSelectedTPCMenu.addr) = 2; //Set to TPC Menu
			}
		} else {
			player->PlayerFlags.CanUseCensusMenu = true;
		}

		debug1(entry);
	}

//Fill Song List | player specific save code
	void FillSongs(MenuEntry *entry) {
		ACNL_Player *player = Player::GetSaveData();
		Language *language = Language::getInstance();

		if(!player) {
			MessageBox(language->get(TextID::SAVE_PLAYER_NO)).SetClear(ClearScreen::Top)();
			return;
		}

		const std::vector<std::string> songopt = {
			language->get(TextID::VECTOR_ENZY_FILL),
			language->get(TextID::VECTOR_ENZY_CLEAR),
		};

		static const std::pair<u16, u16> Pairs = { 0x212B, 0x2186 };

		Keyboard optKb(language->get(TextID::KEY_SONG_LIST_CHOOSE_ACTION), songopt);

		static Address getField(0x2FF76C);

		switch(optKb.Open()) {
			default: break;
			case 0: {
				for(u16 i = Pairs.first; i < Pairs.second; ++i) {
					int field = getField.Call<int>(&i);
					player->AddedSongs[(field >> 5)] |= (1 << (field & 0x1F));
				}
				MessageBox(language->get(TextID::SONG_LIST_FILL_SUCCESS)).SetClear(ClearScreen::Top)();
			} break;
			case 1:
				for(u16 i = Pairs.first; i < Pairs.second; ++i) {
					int field = getField.Call<int>(&i);
					player->AddedSongs[(field >> 5)] &= ~(1 << (field & 0x1F));
				}
				MessageBox(language->get(TextID::SONG_LIST_CLEAR_SUCCESS)).SetClear(ClearScreen::Top)();
			break;
		}
	}

//Fill Catalog | player specific save code
	void FillCatalog(MenuEntry *entry) {
		ACNL_Player *player = Player::GetSaveData();
		Language *language = Language::getInstance();

		if(!player) {
			MessageBox(language->get(TextID::SAVE_PLAYER_NO)).SetClear(ClearScreen::Top)();
			return;
		}

		const std::vector<std::string> songopt = {
			language->get(TextID::VECTOR_ENZY_FILL),
			language->get(TextID::VECTOR_ENZY_CLEAR),
		};

		static const Item_Category CatalogID[15] = {
			Item_Category::Wallpaper, Item_Category::Carpets,
			Item_Category::Furniture, Item_Category::Shirts,
			Item_Category::Dresses, Item_Category::Trousers,
			Item_Category::Socks, Item_Category::Shoes,
			Item_Category::Hats, Item_Category::Accesories,
			Item_Category::Umbrellas, Item_Category::MailPapers,
			Item_Category::Songs, Item_Category::Gyroids,
			Item_Category::AnalyzedFossils
		};

		Keyboard optKb(language->get(TextID::KEY_CATALOG_CHOOSE_ACTION), songopt);

		switch(optKb.Open()) {
			default: break;
			case 0:
				for(int i = 0; i < 15; ++i) {
					Player::SetUnlockableBitField(player, CatalogID[i], true);
				}
				MessageBox(language->get(TextID::CATALOG_FILL_SUCCESS)).SetClear(ClearScreen::Top)();
			break;
			case 1:
				for(int i = 0; i < 15; ++i) {
					Player::SetUnlockableBitField(player, CatalogID[i], false);
				}
				MessageBox(language->get(TextID::CATALOG_CLEAR_SUCCESS)).SetClear(ClearScreen::Top)();
			break;
		}
    }

//Unlock QR Machine | half player specific save code
	void unlockqrmachine(MenuEntry *entry) {
		ACNL_Player *player = Player::GetSaveData();
		ACNL_TownData *town = Town::GetSaveData();
		Language *language = Language::getInstance();

		if(!player || !town) {
			MessageBox(language->get(TextID::SAVE_PLAYER_NO)).SetClear(ClearScreen::Top)();
			return;
		}

		const std::vector<std::string> cmnOpt =  {
			language->get(TextID::VECTOR_ENABLE),
			language->get(TextID::VECTOR_DISABLE)
		};

		Keyboard optKb(language->get(TextID::KEY_QR_MACHINE_CHOOSE_ACTION), cmnOpt);

		int op = optKb.Open();
		if(op < 0) {
			return;
		}

		bool enable = op == 0;

		town->TownFlags.QRMachineUnlocked = enable;

		player->PlayerFlags.BefriendSable1 = enable;
		player->PlayerFlags.BefriendSable2 = enable;
		player->PlayerFlags.BefriendSable3 = enable;

		MessageBox(language->get(enable ? TextID::QR_MACHINE_ENABLE_SUCCESS : TextID::QR_MACHINE_DISABLE_SUCCESS)).SetClear(ClearScreen::Top)();
	}
}