#include <llapi/LoggerAPI.h>
#include <llapi/HookAPI.h>

#include "ScriptX/ScriptX.h"

#include "module.h"
#include "module_scriptx.h"


struct ContextObject {
	void* vptr;
	JSContext* ctx;
};

extern Logger logger;

static script::ScriptEngine* engine;

// void Scripting::QuickJS::ContextObject::_bindModules(ContextObject*, std::vector<Scripting::ModuleBinding> const &)
THook(void, "?_bindModules@ContextObject@QuickJS@Scripting@@AEAAXAEBV?$vector@UModuleBinding@Scripting@@V?$allocator@UModuleBinding@Scripting@@@std@@@std@@@Z", ContextObject* self, void* bindings) {
	logger.info("ContextObject::_bindModules hooked!");
	original(self, bindings);
	JSContext* ctx = self->ctx;

	JSRuntime* rt = JS_GetRuntime(ctx);
	engine = new script::qjs_backend::QjsEngine(nullptr, [rt,ctx]() { return std::make_pair(rt, ctx); });
	{
		script::EngineScope enter(engine);
		bind(engine);
	}


	/*JSModuleDef* m = JS_INIT_MODULE(ctx, "@custom/test");
	if (m)
		logger.info("module @custom/test added!");*/

};

// Scripting::QuickJS::ContextObject::run(std::string const &,std::string const &)
THook(void*, "?run@ContextObject@QuickJS@Scripting@@QEAA?AVResultAny@3@AEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@0@Z", ContextObject* self, void* a2,std::string const& fileName, std::string const& script) {
	logger.info("ContextObject::run enter!");
	script::EngineScope enter(engine);
	void* ret = original(self,a2, fileName, script);
	logger.info("ContextObject::run exit!");
	return ret;
}

// Scripting::QuickJS::ContextObject::call(Scripting::TypedObjectHandle<Scripting::ClosureType>,entt::meta_any *,uint,entt::meta_type)
THook(void*, "?call@ContextObject@QuickJS@Scripting@@QEAA?AVResultAny@3@U?$TypedObjectHandle@UClosureType@Scripting@@@3@PEAVmeta_any@entt@@IVmeta_type@7@@Z", ContextObject* self, void* a2, void* a3, void* a4,  uint32_t a5, void* a6) {
	logger.info("ContextObject::call enter!");
	script::EngineScope enter(engine);
	void* ret = original(self, a2, a3, a4, a5, a6);
	logger.info("ContextObject::call exit!");
	return ret;
}



