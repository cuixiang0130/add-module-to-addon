#include <llapi/LoggerAPI.h>
#include "module_scriptx.h"

extern Logger logger;

static int fib(int n)
{
    if (n <= 0)
        return 0;
    else if (n == 1)
        return 1;
    else
        return fib(n - 1) + fib(n - 2);
}

static script::Local<script::Value> js_fib(const script::Arguments& args) {
    return script::Number::newNumber(fib(args[0].asNumber().toInt32()));
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

    auto fib = script::Function::newFunction(js_fib);
    engine->set("fib", fib);

}

