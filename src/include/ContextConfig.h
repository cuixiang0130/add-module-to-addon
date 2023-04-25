#pragma onece

#include <string>

namespace Scripting {
	struct ContextConfig {
		std::string minEngineVersion;
		std::string pluginName;
		//module
		std::string uuid;
		int16_t majorVersion;
		int16_t minorVersion;
		int16_t patchVersion;
		std::string str;
		int8_t eval;
	};
}