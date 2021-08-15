#ifndef ACMESSAGEBOX_HPP
#define ACMESSAGEBOX_HPP

#include <CTRPluginFramework.hpp>

namespace CTRPluginFramework {
    namespace ACMSG {
        extern Time     LifeSpan;

		void            Stop(void);
        bool            IsRunning(void); //True = running
        void            Notify(const std::string& str);
	}
}
#endif