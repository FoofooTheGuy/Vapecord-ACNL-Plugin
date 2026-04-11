#include <CTRPluginFramework.hpp>
#include "core/hooks/GameLoopHook.hpp"
#include "core/infrastructure/Address.hpp"
#include "core/game_api/Inventory.hpp"
#include "core/game_api/Game.hpp"

namespace CTRPluginFramework {
    namespace {
        enum class KeyboardTypes : u8 {
            Chat = 0,
            TPCMessage,
            BirthdayWish,
            BFFChat,
            PlayerName,
            TownName,
            VillagerNickname,
            VillagerCatchphrase,
            VillagerGreeting,
            ExhibitName,
            SongRequest,
            DreamPlayerName,
            DreamTownName,
            DesignName,
            SearchForWhat
        };

        Hook		g_cancelKeyboardHook;
        Hook		g_submitKeyboardHook;

        void DisableGameKeyboardHooks(void) {
            static Address SetButton1Text(0x193B28);
            static Address SetButton2Text = SetButton1Text.MoveOffset(0xC);

            SetButton1Text.Unpatch();
            SetButton2Text.Unpatch();

            g_submitKeyboardHook.Disable();
            g_cancelKeyboardHook.Disable();
        }

        void BackToCTRPFAsSubmit(u32 data, u32 state, u32 u0) {
            static Address windowOut(0x849D18);

            const HookContext &curr = HookContext::GetCurrent();
            static Address changeState = Address::decodeARMBranch(curr.targetAddress, curr.overwrittenInstr);

            std::string input;

            u32 realData = data - 0x28;
            u32 next = *(u32 *)(realData + 0xE4);
            u32 length = *(u8 *)(next + 0x18);

            u32 textPointer = realData + 0x58;

            Process::ReadString(textPointer, input, length * sizeof(char16_t), StringFormat::Utf16);

            changeState.Call<void>(data, *(u32 *)windowOut.addr, 0);
            DisableGameKeyboardHooks();

            Keyboard::SubmitCustomQwertyInput(input);
        }

        void BackToCTRPFAsCancel(u32 data, u32 state, u32 u0) {
            const HookContext &curr = HookContext::GetCurrent();
            static Address changeState = Address::decodeARMBranch(curr.targetAddress, curr.overwrittenInstr);

            Keyboard::CancelCustomQwertyInput();
            changeState.Call<void>(data, state, u0);
            DisableGameKeyboardHooks();
        }

        void EnableGameKeyboardHooks(void) {
            static Address SetButton1Text(0x193B28);
            static Address SetButton2Text = SetButton1Text.MoveOffset(0xC);
            static Address FunctionForSubmit(0x1938BC);
            static Address CancelKeyboard(0x1938A8);
            static Address TypeAddr(0x193B1C);

            SetButton1Text.Patch(0xE3A01022); //Cancel
            SetButton2Text.Patch(0xE3A0104F); //Confirm

            g_submitKeyboardHook.Initialize(FunctionForSubmit.addr, (u32)BackToCTRPFAsSubmit);
            g_submitKeyboardHook.SetFlags(USE_LR_TO_RETURN);
            g_submitKeyboardHook.Enable();

            g_cancelKeyboardHook.Initialize(CancelKeyboard.addr, (u32)BackToCTRPFAsCancel);
            g_cancelKeyboardHook.SetFlags(USE_LR_TO_RETURN);
            g_cancelKeyboardHook.Enable();
        }
    };

    void LaunchGameKeyboardAsCustomQwerty(Keyboard &keyboard) {
        static Address menuFlags(0x950D68);
        static Address updateMenu(0x520B5C);
        static Address openMenu(0x6D2B4C);

        u32 baseInvPointer = Game::BaseInvPointer();
        if (baseInvPointer != 0) {
            u32 menuData = *(u32 *)(baseInvPointer + 0xC);
            if (menuData != 0) {
                EnableGameKeyboardHooks();

                *(u8 *)(menuFlags.addr) |= 0x20;
                //menuFlags.Write<u8>(0xDC);
                updateMenu.Call<void>(menuData);

                Sleep(Milliseconds(100));

                *(u8 *)(menuFlags.addr) &= ~0x20;
                openMenu.Call<void>((u8)MenuType::ChatKeyboard, 1);
            }
            return;
        }

        static Address mapData(0x950C4C);
        u32 menuData = *(u32 *)mapData.addr;
        if (menuData != 0) { //Otherwise, check if map is open
            EnableGameKeyboardHooks();

            *(u8 *)(menuFlags.addr) |= 0x20;

            //menuFlags.Write<u8>(0xDC);
            updateMenu.Call<void>(menuData);

            Sleep(Milliseconds(100));

            *(u8 *)(menuFlags.addr) &= ~0x20;
            openMenu.Call<void>((u8)MenuType::ChatKeyboard, 1);
            return;
        }

        EnableGameKeyboardHooks();
        openMenu.Call<void>((u8)MenuType::ChatKeyboard, 1); //Nothing open, just open keyboard
    }
};