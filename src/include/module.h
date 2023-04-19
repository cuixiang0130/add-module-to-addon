#pragma once

#include <llapi/LoggerAPI.h>
#include "quickjs.h"

JSModuleDef* JS_INIT_MODULE(JSContext* ctx, const char* module_name);