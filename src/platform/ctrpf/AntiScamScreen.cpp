#include "core/Config.hpp"
#include "Files.h"

namespace CTRPluginFramework {
    namespace {
        static const Color kBgNavy          (28,  20,  52);     //deep purple background
        static const Color kHeaderBlue      (100, 50,  170);    //purple header bar
        static const Color kHeaderHighlight (140, 80,  200);    //lighter purple accent on header
        static const Color kContentBg       (40,  28,  72);     //slightly lighter purple for content
        static const Color kFooterGray      (48,  36,  64);     //dark purple-tinted footer bar
        static const Color kSeparator       (140, 90,  200);    //thin separator line colour
        static const Color kTextLight       (210, 200, 230);    //secondary/dimmed text
        static const Color kAccentTeal      (180, 130, 255);    //lilac accent (link/highlight)
        static const Color kWarningYellow   (255, 210, 60);     //! icon colour
        static const Color kButtonFill      (90,  50,  150);    //A-button prompt bg

        static constexpr u32 kTopW  = 400;
        static constexpr u32 kTopH  = 240;
        static constexpr u32 kBotW  = 320;
        static constexpr u32 kBotH  = 240;

        static void DrawCenteredSysfont(const Screen &scr, const std::string &text, u32 y, const Color &color, u32 screenWidth) {
            u32 textW = Screen::SystemFontSize(text.c_str());
            u32 x = (textW < screenWidth) ? (screenWidth - textW) / 2 : 4;
            scr.DrawSysfont(text, x, y, color);
        }

        static void DrawTopScreen(const Screen &scr) {
            scr.DrawRect(0, 0, kTopW, kTopH, kBgNavy);

            scr.DrawRect(0, 0, kTopW, 30, kHeaderBlue);
            scr.DrawRect(0, 28, kTopW, 2, kHeaderHighlight);

            DrawCenteredSysfont(scr, "! Important Notice !", 6, kWarningYellow, kTopW);

            constexpr u32 boxX = 15, boxY = 37;
            constexpr u32 boxW = kTopW - 2 * boxX, boxH = 164;
            scr.DrawRect(boxX, boxY, boxW, boxH, kContentBg);

            scr.DrawRect(boxX, boxY, boxW, 1, kSeparator);
            scr.DrawRect(boxX, boxY + boxH - 1, boxW, 1, kSeparator);
            scr.DrawRect(boxX, boxY, 1, boxH, kSeparator);
            scr.DrawRect(boxX + boxW - 1, boxY, 1, boxH, kSeparator);

            u32 ty = boxY + 10;
            DrawCenteredSysfont(scr, "This plugin is free and open-source.", ty, Color::White, kTopW);
            ty += 20;
            DrawCenteredSysfont(scr, "It must not be sold, neither alone", ty, Color::White, kTopW);
            ty += 18;
            DrawCenteredSysfont(scr, "nor as part of a bundle.", ty, Color::White, kTopW);
            ty += 26;
            DrawCenteredSysfont(scr, "If you paid for this plugin or received", ty, kTextLight, kTopW);
            ty += 18;
            DrawCenteredSysfont(scr, "it as part of a paid bundle, you have been", ty, kTextLight, kTopW);
            ty += 18;
            DrawCenteredSysfont(scr, "scammed. Demand your money back.", ty, kTextLight, kTopW);
            ty += 24;
            DrawCenteredSysfont(scr, DISCORDINV, ty, kAccentTeal, kTopW);

            scr.DrawRect(0, kTopH - 32, kTopW, 32, kFooterGray);
            scr.DrawRect(0, kTopH - 32, kTopW, 1, kSeparator);

            DrawCenteredSysfont(scr, "Press \uE000 to continue", kTopH - 24, Color::White, kTopW);
        }

        static void DrawBottomScreen(const Screen &src) {
            src.DrawRect(0, 0, kBotW, kBotH, kBgNavy);

            src.DrawRect(0, 0, kBotW, 4, kHeaderBlue);

            DrawCenteredSysfont(src, "Vapecord ACNL Plugin", 100, Color::White, kBotW);
            DrawCenteredSysfont(src, "Free \u2022 Open Source",  122, kAccentTeal, kBotW);
        }

        void InitAntiScamScreen(void) {
            bool needsToUnpause = false;
            if (!Process::IsPaused()) {
                Process::Pause();
                needsToUnpause = true;
            }

            const Screen &top = OSD::GetTopScreen();
            const Screen &bot = OSD::GetBottomScreen();

            for (int i = 0; i < 2; ++i) {
                DrawTopScreen(top);
                DrawBottomScreen(bot);
                OSD::SwapBuffers();
            }

            bool pressed = false;
            while (!pressed) {
                Controller::Update();
                pressed = Controller::IsKeyPressed(Key::A);
                Sleep(Milliseconds(16));
            }

            do { 
                Controller::Update(); 
            } while (Controller::GetKeysDown() & Key::A);

            if (needsToUnpause) {
                Process::Play();
            }
        }
    }

    void ShowAntiScamScreen(void) {
        bool shown = false;
        Config::GetScamWarningShown(shown);

        if (shown) {
            return;
        }

        InitAntiScamScreen();
        Config::SetScamWarningShown(true);
    }
}
