#pragma once

#pragma once

#include <llapi/Global.h>
#include <entt/meta/meta.hpp>

#include "ObjectHandle.h"

namespace Scripting {
	class LifetimeRegistry {
	public:
		// ?valid@LifetimeRegistry@Scripting@@QEAA_NAEBUObjectHandle@2@@Z
		MCAPI bool valid(Scripting::ObjectHandle const&);
		// ?resolveAsAny@LifetimeRegistry@Scripting@@QEAA?AVmeta_any@entt@@UObjectHandle@2@@Z
		MCAPI entt::meta_any resolveAsAny(Scripting::ObjectHandle);
	};
}