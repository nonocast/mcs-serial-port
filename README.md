折腾了半天得到的经验就是看手册，看手册，看手册。

nodejs

```js
const debug = require('debug')('app')
const SerialPort = require('serialport')
const Readline = require('@serialport/parser-readline')
const port = new SerialPort("/dev/tty.usbserial-14310", { baudRate: 9600 });

const parser = new Readline()
port.pipe(parser)

parser.on('data', line => debug(`> ${line}`))
```

Serial Port
===========
个人理解：
1. 意图: 与外部交换数据
2. 异步收发
3. 通信双方采用约定频率(baud rate)采集模拟信号(高低电位)进而转为数字量(0/1)
4. 一个个bit传输，9600表示一秒传9600个bit，每1/9600传输一个bit
5. 一个frame中包含8个有效bit，同时需要辅助起始和校验等
