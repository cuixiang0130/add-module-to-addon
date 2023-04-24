# add-module-to-addon

向addon中的script api添加自定义模块的一个测试

## Minecraft 绑定native层的细节
### 对于对象
通过JS_SetOpaque可以给JSValue挂上一个void*  
然而有点难绷的是  
Minecraft给对象挂上去的并不是一个指针，而是一个64位**整数**  
这个整数高8位是ContextId，低56位是ObjectHandleValue  
用这个整数构造一个ObjectHandle  
通过对当前的JSContext调用JS_GetContextOpaque可以拿到ContextUserData*  
调用其getLifetimeRegistry()拿到LifetimeRegistry  
先通过valid检验ObjectHandle是否有效  
再通过LifetimeRegistry的resolveAsAny可以拿到一个entt::meta_any  
进一步就可以拿到native层的对象了  
### 对于成员函数以及Getter和Setter
JS_NewCFunctionData可以传一个JSValue*,调用注册的func的时候就能拿到
Minecraft用的是一个ArrayBuffer  
然而还是有点难绷的是  
这个ArrayBuffer在是成员函数时是8个字节，但其内容是其实一个**指针** Scripting::Reflection::IFunction *  
不过这主要因为是JS没有64位整数，64位整数会变成浮点数  
在是Getter和Setter时是16个字节，兴许是两个指针罢  
未完待续
## 测试QuickJs API
通过hook函数
```c++
void Scripting::QuickJS::ContextObject::_bindModules(ContextObject*, std::vector<Scripting::ModuleBinding> const &)
```
从ContextObject拿到JSContext并在原本的模块注册逻辑后注册自定义模块  
由于PeEditor忽略了所有C函数符号，暂时需要使用这个[fork](https://github.com/cuixiang0130/PeEditor)  
注册的模块似乎要以@开头才会被加载  
行为包script api 测试  
```javascript
import { fib, info } from '@custom/test'
import { system } from '@minecraft/server'

system.runInterval(()=>{
    info("test fib(10):" + fib(10))
},100)
```
![image](https://github.com/cuixiang0130/add-module-to-addon/blob/main/result.jpg)

## 测试ScriptX API
JS_NewClassID函数与原本QuickJs发生了变化，需要修改ScriptX
```c++
JSClassID JS_NewClassID(JSRuntime* rt, JSClassID *pclass_id);
```
部分API被移除，需要手动加入回来
```c++
void* JS_GetOpaque(JSValueConst obj, JSClassID class_id);
JSValue JS_NewArrayBuffer(JSContext* ctx, uint8_t* buf, size_t len,
    JSFreeArrayBufferDataFunc* free_func, void* opaque,
    BOOL is_shared);
BOOL JS_IsConstructor(JSContext* ctx, JSValueConst val);
BOOL JS_SetConstructorBit(JSContext* ctx, JSValueConst func_obj, BOOL val);
void JS_UpdateStackTop(JSRuntime* rt);
```
~~hook运行脚本的函数，保证调用native函数时在script::EngineScope内~~
```c++
Scripting::QuickJS::ContextObject::run(std::string const &,std::string const &);
Scripting::QuickJS::ContextObject::call(Scripting::TypedObjectHandle<Scripting::ClosureType>,entt::meta_any *,uint,entt::meta_type);
```
修改ScriptX *newFunction* 和 *newRawFunction* 调用callback前加个EngineScope

测试js  
main.js
```javascript
import { system, world } from '@minecraft/server'
import { moduleTest } from './ModuleTest.js'

Logger.info("main :fib(10):" + fib(10))

system.runInterval(()=>{
    Logger.info("runInterval :fib(10):" + fib(10))
},100)

moduleTest("module func used in main :");
```
ModuleTest.js
```javascript
export function moduleTest(str){
    Logger.info(str + "test fib(10):" + fib(10))
}

moduleTest("module load side effect :");
```

![image](https://github.com/cuixiang0130/add-module-to-addon/blob/main/result_scriptx.jpg)