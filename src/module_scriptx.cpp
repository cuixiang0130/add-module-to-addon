#include "llapi/LoggerAPI.h"
#include "llapi/mc/Player.hpp"
#include "entt/core/hashed_string.hpp"

#include "module_scriptx.h"
#include "QjsHelper.hpp"
#include "ObjectHandle.h"
#include "ContextUserData.h"

extern Logger logger;

namespace ScriptModuleMinecraft{
    class ScriptPlayer{
    public:
        //?tryGetPlayer@ScriptPlayer@ScriptModuleMinecraft@@QEBAPEAVPlayer@@XZ
        MCAPI Player* tryGetPlayer(void) const;
    };
}
namespace Scripting{
    namespace QuickJS{
        // ?JSValueToNativeAny@QuickJS@Scripting@@YA?AV?$variant@Vmeta_any@entt@@UJSValue@@@std@@PEAUJSContext@@UJSValue@@Vmeta_type@entt@@@Z
        MCAPI std::variant<entt::meta_any,JSValue> JSValueToNativeAny(JSContext*, JSValue, entt::meta_type);
    }
}

static script::Local<script::Value> js_get_ip(const script::Arguments& args) {

    using namespace script;
    using namespace entt;
    auto ctx = qjs_interop::currentContext();
    auto val = qjs_interop::getLocal(args[0], ctx);
    auto any = Scripting::QuickJS::JSValueToNativeAny(ctx, val, resolve<ScriptModuleMinecraft::ScriptPlayer>());
    if (any.index() == 0) {
        auto splayer = std::get<meta_any>(any).try_cast<ScriptModuleMinecraft::ScriptPlayer>();
        auto player = splayer->tryGetPlayer();
        auto ip = player->getIP();
        return String::newString(ip);
    }
    
    return Local<Value>();
}


static script::Local<script::Value> js_native(const script::Arguments& args) {

    using namespace script;
    auto ctx = qjs_interop::currentContext();
    auto val = qjs_interop::getLocal(args[0], ctx);
    uint64_t ptr = (uint64_t)JS_GetOpaqueWithoutClass(val);
    auto ctxd = (Scripting::QuickJS::ContextUserData*)JS_GetContextOpaque(ctx);
    Scripting::LifetimeRegistry& reg = ctxd->getLifetimeRegistry();
    Scripting::ObjectHandle handle(ptr);
    if (reg.valid(handle)) {
        auto any = reg.resolveAsAny(handle);
        auto type = any.type().info().name();
        return String::newString(type);
    }	
    return Local<Value>();
}

script::Local<script::Value> TestClass::info(const script::Arguments& args){
    logger.info(args[0].asString().toString());
    return script::Local<script::Value>();
}


void bind(script::ScriptEngine* engine) {

    static script::ClassDefine<TestClass> testDef =
        script::defineClass<TestClass>("Logger")
        .constructor(nullptr)
        .function("info", &TestClass::info)
        .build();

    engine->registerNativeClass(testDef);

    auto native = script::Function::newFunction(js_native);
    auto getIP = script::Function::newFunction(js_get_ip);
    engine->set("native", native);
    engine->set("getIP", getIP);
}

