//import { fib, info } from '@custom/test'
import { system, world } from '@minecraft/server'
import { moduleTest } from './ModuleTest.js'

Logger.info("main :fib(10):" + fib(10))

system.runInterval(()=>{
    Logger.info("runInterval :fib(10):" + fib(10))
},100)

moduleTest("module func used in main :");