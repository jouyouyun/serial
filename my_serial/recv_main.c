/*
 * 读取串口数据
 */

#include "my_serial.h"

int
main( int argc, char *argv[] )
{
	struct port_info info;

	if ( argc != 2 ) {
		debug( "%s /dev/ttySn\n", argv[0] );
		return -1;
	}

	port_fd = open_port( argv[1] );
	if ( port_fd == -1 ) {
		debug( "open_port err" );
		return -1;
	}
	atexit(close_port);

	/* 设置串口参数 */
	info.baud_rate = 38400;
	info.data_bits = 8;
	info.flow_ctrl = 2;
	info.port_fd = port_fd;
	info.stop_bit = 1;
	info.parity = 0;

	if ( set_port( &info ) == -1 ) {
		debug( "set_port err\n" );
		return -1;
	}
	recv_msg(&info);

	return 0;
}
