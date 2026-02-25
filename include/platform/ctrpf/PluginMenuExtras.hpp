#pragma once

#include <CTRPluginFramework.hpp>
#include "core/infrastructure/Language.hpp"

namespace CTRPluginFramework {
    class PluginMenuExtras {
    public:
        /**
         * \brief Update all folders and entries (their names, notes and colors)
         */
        static void    Update();
    };
}