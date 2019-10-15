折腾了半天得到的经验就是看手册，看手册，看手册。

nodejs

```
const debug = require('debug')('app')
const SerialPort = require('serialport')
const Readline = require('@serialport/parser-readline')
const port = new SerialPort("/dev/tty.usbserial-14310", { baudRate: 9600 });

const parser = new Readline()
port.pipe(parser)

parser.on('data', line => debug(`> ${line}`))
```
