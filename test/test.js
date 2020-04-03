let serial = require('../lib');
let x = new serial.serialport('com3:', {});
x.write('1254');
