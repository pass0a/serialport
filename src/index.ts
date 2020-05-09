import { Duplex } from 'stream';
import { isBuffer } from 'util';

let bindings = require('bingdings')('serialport');
export interface UartOpts {
	flow_control?: number;
	baud_rate?: number;
	parity?: number;
	stop_bits?: number;
	character_size?: number;
	timeout?: number;
	[header: string]: string | number | boolean | undefined;
}
interface PUartOpts {
	flow_control: number;
	baud_rate: number;
	parity: number;
	stop_bits: number;
	character_size: number;
	timeout: number;
	[header: string]: string | number | boolean | undefined;
}
export class serialport extends Duplex {
	private handle: any;
	private opt: PUartOpts;
	constructor(name: string, options?: UartOpts) {
		super({ objectMode: true });
		this.opt = {
			flow_control: 0,
			baud_rate: 115200,
			parity: 0,
			stop_bits: 0,
			character_size: 8,
			timeout: 500
		};
		for (const key in options) {
			if (options.hasOwnProperty(key)) {
				this.opt[key] = options[key];
			}
		}
		console.log(this instanceof serialport);
		this.handle = bindings.open(name, options, this.onEvent.bind(this));
		this.on('error', (code: number, msg: string) => {
			console.log('error in serial_port:', code, msg);
		});
		console.log(name);
	}
	onEvent(ev: string, ...args: any[]) {
		console.log(ev);
		this.emit(ev, ...args);
	}
	_write(chunk: any, encoding: string, callback: (error?: Error | null) => void) {
		if (!isBuffer(chunk)) {
			chunk = Buffer.from(chunk);
		}
		bindings.write(this.handle, chunk);
		callback();
	}
	_final() {
		bindings.close(this.handle);
	}
}
