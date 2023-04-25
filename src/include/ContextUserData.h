#pragma once

#include <llapi/Global.h>
#include <LifetimeRegistry.h>
#include <ContextConfig.h>

namespace Scripting {
	namespace QuickJS {
		class ContextUserData {
		public:
			uint8_t contextId;
			// ?getLifetimeRegistry@ContextUserData@QuickJS@Scripting@@QEAAAEAVLifetimeRegistry@3@XZ
			MCAPI LifetimeRegistry& getLifetimeRegistry();
			// ?getContextConfig@ContextUserData@QuickJS@Scripting@@QEBAAEBUContextConfig@3@XZ
			MCAPI const ContextConfig & getContextConfig() const;
		};
	};
}