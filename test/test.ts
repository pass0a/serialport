import { serialport } from '../src/index';
let s = new serialport('COM1', { baud_rate: 9600 });
s.on('data', (data) => {
	console.log(data);
	s.write('nihao');
});
