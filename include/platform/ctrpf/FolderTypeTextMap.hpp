#pragma once

#include "platform/ctrpf/FolderTypes.hpp"
#include "core/infrastructure/TextID.hpp"

namespace CTRPluginFramework {
    inline TextID GetFolderTextId(FolderType type) {
        switch (type) {
            case FolderType::Save:        return TextID::SAVE_CODES;
            case FolderType::Movement:    return TextID::MOVEMENT_CODES;
            case FolderType::Inventory:   return TextID::INVENTORY_CODES;
            case FolderType::Player:      return TextID::PLAYER_CODES;
            case FolderType::Animation:   return TextID::ANIMATION_CODES;
            case FolderType::Seeding:     return TextID::SEEDING_CODES;
            case FolderType::Money:       return TextID::MONEY_CODES;
            case FolderType::Island:      return TextID::ISLAND_CODES;
            case FolderType::NPC:         return TextID::NPC_CODES;
            case FolderType::Environment: return TextID::ENV_CODES;
            case FolderType::Extra:       return TextID::EXTRA_CODES;
            case FolderType::Misc:        return TextID::MISC_CODES;
            case FolderType::Default:     return TextID::DEFAULT_CODES;
            case FolderType::Dev:         return TextID::DEV_CODES;
            case FolderType::Pretendo:    return TextID::PRETENDO_CODES;
            default:                      return TextID::NONE;
        }
    }

    inline TextID GetFolderNoteTextId(FolderType type) {
        switch (type) {
            case FolderType::Save:        return TextID::SAVE_CODES_NOTE;
            case FolderType::Movement:    return TextID::MOVEMENT_CODES_NOTE;
            case FolderType::Inventory:   return TextID::INVENTORY_CODES_NOTE;
            case FolderType::Player:      return TextID::PLAYER_CODES_NOTE;
            case FolderType::Animation:   return TextID::ANIMATION_CODES_NOTE;
            case FolderType::Seeding:     return TextID::SEEDING_CODES_NOTE;
            case FolderType::Money:       return TextID::MONEY_CODES_NOTE;
            case FolderType::Island:      return TextID::ISLAND_CODES_NOTE;
            case FolderType::NPC:         return TextID::NPC_CODES_NOTE;
            case FolderType::Environment: return TextID::ENV_CODES_NOTE;
            case FolderType::Extra:       return TextID::EXTRA_CODES_NOTE;
            case FolderType::Misc:        return TextID::MISC_CODES_NOTE;
            case FolderType::Default:     return TextID::DEFAULT_CODES_NOTE;
            case FolderType::Dev:         return TextID::DEV_CODES_NOTE;
            case FolderType::Pretendo:    return TextID::PRETENDO_CODES_NOTE;
            default:                      return TextID::NONE;
        }
    }

    inline TextID GetSubFolderTextId(FolderType parent, SubFolder sub) {
        if (sub == SubFolder::None)
            return TextID::NONE;

        switch (parent) {
            case FolderType::Player:
                if (sub == SubFolder::PlayerSave)
                    return TextID::PLAYER_SAVE_CODES;
                break;

            case FolderType::Seeding:
                switch (sub) {
                    case SubFolder::Seed: return TextID::SEED_CODES;
                    case SubFolder::Drop: return TextID::DROP_CODES;
                    case SubFolder::Tree: return TextID::TREE_CODES;
                    default: break;
                }
                break;

            case FolderType::Environment:
                switch (sub) {
                    case SubFolder::Fish: return TextID::FISH_CODES;
                    case SubFolder::Insect: return TextID::INSECT_CODES;
                    default: break;
                }
                break;

            case FolderType::Extra:
                switch (sub) {
                    case SubFolder::Chat: return TextID::CHAT_CODES;
                    case SubFolder::Fun: return TextID::FUN_CODES;
                    default: break;
                }
                break;

            case FolderType::Pretendo:
                switch (sub) {
                    case SubFolder::PlayersInSession: return TextID::PLAYERS_IN_SESSION;
                    case SubFolder::PIALoggerOptions: return TextID::PIA_LOGGER_OPTIONS;
                    default: break;
                }
                break;

            default:
                break;
        }

        return TextID::NONE;
    }

    inline TextID GetSubFolderNoteTextId(FolderType parent, SubFolder sub) {
        if (sub == SubFolder::None)
            return TextID::NONE;

        switch (parent) {
            case FolderType::Player:
                if (sub == SubFolder::PlayerSave)
                    return TextID::PLAYER_SAVE_CODES_NOTE;
                break;

            case FolderType::Seeding:
                switch (sub) {
                    case SubFolder::Seed: return TextID::SEED_CODES_NOTE;
                    case SubFolder::Drop: return TextID::DROP_CODES_NOTE;
                    case SubFolder::Tree: return TextID::TREE_CODES_NOTE;
                    default: break;
                }
                break;

            case FolderType::Environment:
                switch (sub) {
                    case SubFolder::Fish: return TextID::FISH_CODES_NOTE;
                    case SubFolder::Insect: return TextID::INSECT_CODES_NOTE;
                    default: break;
                }
                break;

            case FolderType::Extra:
                switch (sub) {
                    case SubFolder::Chat: return TextID::CHAT_CODES_NOTE;
                    case SubFolder::Fun: return TextID::FUN_CODES_NOTE;
                    default: break;
                }
                break;

            case FolderType::Pretendo:
                switch (sub) {
                    case SubFolder::PlayersInSession: return TextID::PLAYERS_IN_SESSION_NOTE;
                    case SubFolder::PIALoggerOptions: return TextID::PIA_LOGGER_OPTIONS_NOTE;
                    default: break;
                }
                break;

            default:
                break;
        }

        return TextID::NONE;
    }
}
