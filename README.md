# add-module-to-addon

向addon中的script api添加自定义模块的一个测试

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