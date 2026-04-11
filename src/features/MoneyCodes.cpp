#include "features/cheats.hpp"
#include "core/game_api/Player.hpp"
#include "core/game_api/Game.hpp"
#include "core/infrastructure/PluginUtils.hpp"
#include "core/infrastructure/Address.hpp"
#include "core/game_api/Town.hpp"

namespace CTRPluginFramework {
//Wallet Mod
	void wallet(MenuEntry *entry) {
		ACNL_Player *player = Player::GetSaveData();

		if(!player) {
			MessageBox(Language::getInstance()->get(TextID::SAVE_PLAYER_NO)).SetClear(ClearScreen::Top)();
			return;
		}

		u32 money = 0;
		Keyboard kb(Language::getInstance()->get(TextID::ENTER_AMOUNT));
		kb.SetSlider(0, 99999, 100);
		kb.IsHexadecimal(false);
		if(kb.Open(money) >= 0) {
			Game::EncryptValue(&player->PocketMoney, money);
		}
	}
//Bank Mod
	void bank(MenuEntry *entry) {
		ACNL_Player *player = Player::GetSaveData();

		if(!player) {
			MessageBox(Language::getInstance()->get(TextID::SAVE_PLAYER_NO)).SetClear(ClearScreen::Top)();
			return;
		}

		u32 money = 0;
		Keyboard kb(Language::getInstance()->get(TextID::ENTER_AMOUNT));
		kb.SetSlider(0, 999999999, 10000);
		kb.IsHexadecimal(false);
		if(kb.Open(money) >= 0) {
			Game::EncryptValue(&player->BankAmount, money);
		}
	}
//Meow Coupon Mod
	void coupon(MenuEntry *entry) {
		ACNL_Player *player = Player::GetSaveData();

		if(!player) {
			MessageBox(Language::getInstance()->get(TextID::SAVE_PLAYER_NO)).SetClear(ClearScreen::Top)();
			return;
		}

		u32 coupon = 0;
		Keyboard kb(Language::getInstance()->get(TextID::ENTER_AMOUNT));
		kb.SetSlider(0, 9999, 10);
		kb.IsHexadecimal(false);
		if(kb.Open(coupon) >= 0) {
			Game::EncryptValue(&player->MeowCoupons, coupon);
		}
	}
//Badges Mod
	void badges(MenuEntry *entry) {
		ACNL_Player *player = Player::GetSaveData();

		if(!player) {
			MessageBox(Language::getInstance()->get(TextID::SAVE_PLAYER_NO)).SetClear(ClearScreen::Top)();
			return;
		}

		const std::vector<std::string> badgesopt = {
			Color(0xFFD700FF) << Language::getInstance()->get(TextID::VECTOR_BADGE_GOLD),
			Color(0xC0C0C0FF) << Language::getInstance()->get(TextID::VECTOR_BADGE_SILVER),
			Color(0xCD7F32FF) << Language::getInstance()->get(TextID::VECTOR_BADGE_BRONZE),
			Language::getInstance()->get(TextID::VECTOR_BADGE_NONE)
		};

		Keyboard optKb(Language::getInstance()->get(TextID::KEY_CHOOSE_OPTION), badgesopt);

		int index = optKb.Open();
		if(index < 0) {
			return;
		}

		bool WithStats = MessageBox(Language::getInstance()->get(TextID::BADGE_SETTER_STATS_QUESTION), DialogType::DialogYesNo).SetClear(ClearScreen::Top)();

		for(int i = 0; i < 24; ++i) {
			Game::SetBadges(i, std::abs(index - 3), WithStats);
		}
	}
//Medals Mod  32DC51B8 31F2C6BC
	void medals(MenuEntry *entry) {
		ACNL_Player *player = Player::GetSaveData();

		if(!player) {
			MessageBox(Language::getInstance()->get(TextID::SAVE_PLAYER_NO)).SetClear(ClearScreen::Top)();
			return;
		}

		u32 medal = 0;
		Keyboard kb(Language::getInstance()->get(TextID::ENTER_AMOUNT));
		kb.SetSlider(0, 9999, 10);
		kb.IsHexadecimal(false);
		if(kb.Open(medal) >= 0) {
			Game::EncryptValue(&player->MedalAmount, medal);
		}
	}
//turnip Mod
	void turnips(MenuEntry *entry) {
		ACNL_TownData *town = Town::GetSaveData();

		if(!town) {
			MessageBox(Language::getInstance()->get(TextID::SAVE_PLAYER_NO)).SetClear(ClearScreen::Top)();
			return;
		}

		u32 turnip = 0;
		Keyboard kb(Language::getInstance()->get(TextID::ENTER_AMOUNT));
		kb.SetSlider(0, 99999, 10);
		kb.IsHexadecimal(false);
		if(kb.Open(turnip, turnip) == 0) {
			for(int i = 0; i < 6; ++i) {
				Game::EncryptValue(&town->TurnipPrices[i], turnip); //AM
				Game::EncryptValue(&town->TurnipPrices[i + 6], turnip); //PM
			}
		}
	}
}