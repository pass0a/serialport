import { serialport } from '../src/index';
let s = new serialport('COM1');
s.on('data', (data) => {
	console.log(data);
	s.write('nihao');
});
