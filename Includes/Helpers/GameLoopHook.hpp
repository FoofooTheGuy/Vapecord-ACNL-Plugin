#pragma once

#include <CTRPluginFramework.hpp>
#include "Address/Address.hpp"

namespace CTRPluginFramework {
	class GameLoopHook {
	public:
		static GameLoopHook* GetInstance() {
			static GameLoopHook instance;
			return &instance;
		}

		//returns `false` if `action` is already in progress
		bool Add(bool(*action)()) {
			Lock _(lock);
			if (std::find(actions.begin(), actions.end(), action) == actions.end()) {
				actions.push_back(action);
				return true;
			}
			return false;
		}

	private:
		static void LoopHookMethod(u32 u0) {
			auto* instance = GetInstance();
			{
				Lock _(instance->lock);
				std::erase_if(instance->actions, [](const auto& action) {
					return action();
				});
			}
			const HookContext &curr = HookContext::GetCurrent();
			static Address func = Address::decodeARMBranch(curr.targetAddress, curr.overwrittenInstr);
			func.Call<void>(u0);
		}

		Address loopToHookOnto {0x54DB00};
		LightLock lock {};
		std::vector<bool(*)()> actions;
		Hook hook = [this] {
			Hook result;
			result.Initialize((u32)loopToHookOnto.addr, (u32)LoopHookMethod);
			result.SetFlags(USE_LR_TO_RETURN);
			result.Enable();
			return result;
		}();
	};
}
