# add-module-to-addon

向addon中的script api添加自定义模块的一个测试

## Minecraft脚本引擎的上下文对象
给JSContext通过SetContextOpaque挂上去的是ContextUserData*  
JSRuntime的暂不明，好像是跟Promise相关的

## 构造Engine  
hook注入点位，绑定各个@minecraft模块的函数调用后（或许有更好的点位，再看吧）  
```c++
void Scripting::QuickJS::ContextObject::_bindModules(ContextObject*, std::vector<Scripting::ModuleBinding> const &)
```
从ContextObject拿到JSContext构造ScriptX的Engine  
还能拿到ContextUserData，通过ContextUserData又能拿到ContextConfig，可以得到plugin的uuid之类的信息，可以类似与BDS的方式做能否访问模块的权限管理，按需加载

## 销毁Engine  
暂不清楚JSContext除了在服务器关闭时以外会销毁重建吗，也许reload会重建，需要修改ScriptX的destroy逻辑  

## 与Minecraft模块生成的对象的转换  
通过以下两个函数转换，或者手动通过LifetimeRegistry转换  
```c++
std::variant<entt::meta_any,JSValue> Scripting::QuickJS::JSValueToNativeAny(JSContext*, JSValue, entt::meta_type);
JSValue Scripting::QuickJS::NativeAnyToJSValue(JSContext*, entt::meta_any&, bool, bool);
```

## Minecraft脚本引擎对象管理  
Minecraft的脚本引擎的native对象都通过LifetimeRegistry管理，每个JSValue都会通过JS_SetOpaque可以给JSValue挂上一个void*，然而有点难绷的，挂上去的并不是一个指针，而是一个64位**整数**  ，这个整数高8位是ContextId，低56位是ObjectHandleValue，用这个整数可以构造一个ObjectHandle进一步去调用LifetimeRegistry的函数，有StrongObjectHandle和WeakObjectHandle看上去是对应强弱引用  

### 获取对象
获取ObjectHandle后，先通过LifetimeRegistry的valid函数检验ObjectHandle是否有效，再通过LifetimeRegistry的resolveAsAny可以拿到一个entt::meta_any，进一步就可以拿到native层的对象了  

### 创建对象
未完待续  

## Minecraft脚本引擎函数绑定
对于成员函数以及Getter和Setter是通过JS_NewCFunctionData生成的，该函数可以传一个JSValue*,调用注册的func的时候就能拿到
Minecraft用的是一个ArrayBuffer，然而还是有点难绷的是，这个ArrayBuffer在是成员函数时是8个字节，但其内容是其实一个**指针** Scripting::Reflection::IFunction *  
不过这主要因为是JS没有64位整数，64位整数会变成浮点数  
在是Getter和Setter时是16个字节，兴许是两个指针罢  
函数调用时有Privilege的检查，暂时不清楚指的是什么。PrivilegeScope会给ContextUserData设置需要的privilege，析构后会改回去，调用时通过ContextUserData拿到RequiredPrivilege进行检查。暂不清楚无视Privilege调用会怎么样

## 与原Quickjs的API变化
JS_NewClassID函数与原本QuickJs发生了变化，需要修改ScriptX，还有没有其它的函数暂时不明
```c++
JSClassID JS_NewClassID(JSRuntime* rt, JSClassID *pclass_id);
```
因为没有使用或内联被移除的函数，要用ScriptX需要手动加入回来
```c++
void* JS_GetOpaque(JSValueConst obj, JSClassID class_id);
JSValue JS_NewArrayBuffer(JSContext* ctx, uint8_t* buf, size_t len,
    JSFreeArrayBufferDataFunc* free_func, void* opaque,
    BOOL is_shared);
BOOL JS_IsConstructor(JSContext* ctx, JSValueConst val);
BOOL JS_SetConstructorBit(JSContext* ctx, JSValueConst func_obj, BOOL val);
void JS_UpdateStackTop(JSRuntime* rt);
```

## 符号链接
由于LL还要带一个QuickJS，要动态确定函数地址，暂时先拿这个凑合用[fork](https://github.com/cuixiang0130/PeEditor)  

## 模块注册与加载
Minecraft自定义了模块名的Normalize和模块加载的函数
注册的模块以@开头才会被认为是C模块,逻辑大概在这个函数里
```c++
ScriptPluginUtils::JavaScriptModuleNameNormalizer(std::string_view const &,std::string const &,std::vector<std::string> const &)
```

## 自动进入EngineScope
保证调用native函数时要在script::EngineScope内，run是加载执行时调用，call是调用各种事件的callback时调用
```c++
Scripting::QuickJS::ContextObject::run(std::string const &,std::string const &);
Scripting::QuickJS::ContextObject::call(Scripting::TypedObjectHandle<Scripting::ClosureType>,entt::meta_any *,uint,entt::meta_type);
```
与其去hook这些函数，还不如修改ScriptX *newFunction* 和 *newRawFunction* 调用callback前加个EngineScope

## 使用ScriptX的收益与困难
Minecraft内部也实现了一个类似的脚本引擎抽象层，以后会不会换v8，会不会加个python之类的不好说，但从Manifest的声明与反编译的代码看是有这个野心的，这样的话，使用ScriptX或许更能应对未来的变化。如果Xbox Game Studio偷懒，那当我没说。  
ScriptX目前不支持导出模块，JSContext的opaque也被Minecraft占了，实现导出模块会稍微复杂一些
ScriptX好像没有Minecraft注册个Iterface转换JS对象这种功能


