#include "module.h"

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

static JSValue js_fib(JSContext* ctx, JSValueConst this_val,
    int argc, JSValueConst* argv)
{
    int n, res;
    if (JS_ToInt32(ctx, &n, argv[0]))
        return JS_EXCEPTION;
    res = fib(n);
    return JS_NewInt32(ctx, res);
}

static JSValue js_info(JSContext* ctx, JSValueConst this_val,
    int argc, JSValueConst* argv)
{
    const char* msg = JS_ToCString(ctx, argv[0]);
    logger.info(msg);
    return JS_UNDEFINED;
}


static int js_custom_module_init(JSContext* ctx, JSModuleDef* m)
{
    JSValue info = JS_NewCFunction2(ctx, js_info, "info", 1, JS_CFUNC_generic, 0);
    JS_SetModuleExport(ctx, m, "info", info);
    JSValue fib = JS_NewCFunction2(ctx, js_fib, "fib", 1, JS_CFUNC_generic, 0);
    JS_SetModuleExport(ctx, m, "fib", fib);
    return 0;
}

JSModuleDef* JS_INIT_MODULE(JSContext* ctx, const char* module_name)
{
    JSModuleDef* m;
    m = JS_NewCModule(ctx, module_name, js_custom_module_init);
    if (!m)
        return NULL;
    JS_AddModuleExport(ctx, m, "info");
    JS_AddModuleExport(ctx, m, "fib");
    return m;
}