#pragma once

#include "ScriptX/ScriptX.h"

class TestClass : public script::ScriptClass {
public:
    using script::ScriptClass::ScriptClass;
    static script::Local<script::Value> info(const script::Arguments& args);
};

void bind(script::ScriptEngine* engine);