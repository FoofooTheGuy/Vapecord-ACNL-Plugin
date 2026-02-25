#pragma once

#include <CTRPluginFramework.hpp>
#include "core/game_api/GameStructs.hpp"

namespace CTRPluginFramework {
	namespace ItemSequence {
		void		Init();
		Item		*Next();
		Item		PeekNext();
		void		Switch(bool enable);
		bool		Enabled();
		void		OpenIS();
	}
}