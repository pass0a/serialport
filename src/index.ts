import { Duplex } from 'stream';
declare function __passoa_net_connect_serialport(
	name: string,
	flow_control: number,
	baud_rate: number,
	parity: number,
	stop_bits: number,
	character_size: number,
	cb: (ev: string, ...args: any[]) => void
): any
declare function __passoa_net_write_serialport(sock: any, chunk: Buffer): number
declare function __passoa_net_close_serialport(sock: any): number

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
		this.handle = __passoa_net_connect_serialport(
			name,
			this.opt.flow_control,
			this.opt.baud_rate,
			this.opt.parity,
			this.opt.stop_bits,
			this.opt.character_size,
			this.onEvent.bind(this)
		);
		this.on('error', (code: number, msg: string) => {
			console.log('error in serial_port:', code, msg);
		});
		console.log(name);
	}
	onEvent(ev: string, ...args: any[]) {
		this.emit(ev, ...args);
	}
	_read() {}
	_write(chunk: any, encoding: string, callback: (error?: Error | null) => void) {
		if (chunk.constructor.name != 'Buffer') {
			chunk = Buffer.from(chunk);
		}
		__passoa_net_write_serialport(this.handle, chunk);
		callback();
	}
	_final() {
		__passoa_net_close_serialport(this.handle);
	}
}
