import { fib, info } from '@custom/test'
import { system } from '@minecraft/server'

system.runInterval(()=>{
    info("test fib(10):" + fib(10))
},100)