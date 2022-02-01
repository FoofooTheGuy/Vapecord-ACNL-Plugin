#ifndef ITEMSEQUENCE_HPP
#define ITEMSEQUENCE_HPP

#include <CTRPluginFramework.hpp>
#include "Helpers/GameStructs.hpp"

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
#endif