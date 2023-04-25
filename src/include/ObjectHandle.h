#pragma once

#include <llapi/Global.h>

namespace Scripting {
	struct ObjectHandle {
		uint64_t objectHandleValue;
		uint8_t contextId;
	public:
		MCAPI ObjectHandle();
		MCAPI ObjectHandle(uint64_t);
	};
}