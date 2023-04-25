#include <llapi/LoggerAPI.h>
#include <llapi/HookAPI.h>

#include "ScriptX/ScriptX.h"
#include "QjsHelper.hpp"
#include "ContextUserData.h"

#include "module.h"
#include "module_scriptx.h"


struct ContextObject {
	void* vptr;
	JSContext* ctx;
};

struct IFunction {
	void* vptr;
	uint32_t privilege;
};

extern Logger logger;


static script::qjs_backend::QjsEngine* engine;

// void Scripting::QuickJS::ContextObject::_bindModules(ContextObject*, std::vector<Scripting::ModuleBinding> const &)
THook(void, "?_bindModules@ContextObject@QuickJS@Scripting@@AEAAXAEBV?$vector@UModuleBinding@Scripting@@V?$allocator@UModuleBinding@Scripting@@@std@@@std@@@Z", ContextObject* self, void* bindings) {
	logger.info("ContextObject::_bindModules hooked!");
	original(self, bindings);
	JSContext* ctx = self->ctx;

	JSRuntime* rt = JS_GetRuntime(ctx);

	auto ctxd = (Scripting::QuickJS::ContextUserData*)JS_GetContextOpaque(ctx);
	const Scripting::ContextConfig& config = ctxd->getContextConfig();
	logger.info("Plugin {0} Loaded",config.pluginName);
	logger.info("Min Engine Version : {0}",config.minEngineVersion);
	logger.info("Module UUID : ",config.uuid);
	logger.info("Module Version : {0}.{1}.{2}", config.majorVersion, config.minorVersion, config.patchVersion);
	logger.info("unknown_str : {0}",config.str);
	logger.info("Eval Enabled : {0}", (bool)config.eval);

	engine = new script::qjs_backend::QjsEngine(nullptr, [rt,ctx]() { return std::make_pair(rt, ctx); });
	{
		script::EngineScope enter(engine);
		bind(engine);
	}

};


