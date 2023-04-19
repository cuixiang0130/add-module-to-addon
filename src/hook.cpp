#include <llapi/LoggerAPI.h>

#include "quickjs.h"
#include "module.h"
#include "llapi/HookAPI.h"

struct ContextObject {
	void* vptr;
	JSContext* ctx;
};

extern Logger logger;

// void Scripting::QuickJS::ContextObject::_bindModules(ContextObject*, std::vector<Scripting::ModuleBinding> const &)
THook(void, "?_bindModules@ContextObject@QuickJS@Scripting@@AEAAXAEBV?$vector@UModuleBinding@Scripting@@V?$allocator@UModuleBinding@Scripting@@@std@@@std@@@Z", ContextObject* self, void* bindings) {
	original(self, bindings);
	logger.info("_bindModules hooked!");
	JSContext* ctx = self->ctx;
	JSModuleDef* m = JS_INIT_MODULE(ctx, "@custom/test");
	if(m)
		logger.info("module @custom/test added!");
};
