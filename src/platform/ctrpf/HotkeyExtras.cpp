#include "platform/ctrpf/HotkeyExtras.hpp"
#include "platform/ctrpf/UtilsExtras.hpp"

namespace CTRPluginFramework {
    HotkeyExtras::HotkeyExtras(u32 keys, const TextID &nameKey)
        : Hotkey(keys, UtilsExtras::setLanguageByKey(nameKey)) {
        this->NameKey = nameKey;
    }
}