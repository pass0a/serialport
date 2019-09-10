import { serialport } from '../src/index';
let s = new serialport('COM7:');
s.on('data', (data) => {
	console.log(data);
});
