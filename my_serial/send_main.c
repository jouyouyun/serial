/*
 * 发送数据
 */

#include "my_serial.h"

int
main( int argc, char *argv[] )
{
	int i;
	int len;
	int port_fd;
	char recv_buf[MAX_BUF_LEN];
	struct port_info info;

	if ( argc != 2 ) {
		fprintf( stdout, "%s /dev/ttySn\n", argv[0] );
		return -1;
	}

	port_fd = open_port( argv[1] );
	if ( port_fd == -1 ) {
		fprintf( stderr, "open_port err" );
		return -1;
	}

	/* 设置串口参数 */
	info.baud_rate = 9600;
	info.data_bits = 8;
	info.flow_ctrl = 2;
	info.port_fd = port_fd;
	info.stop_bit = 1;
	info.parity = 0;

	if ( set_port( &info ) == -1 ) {
		fprintf( stderr, "set_port err\n" );
		close(port_fd);
		return -1;
	}

	/* 发送数据 */
	for ( i = 0; i < 10; i++ ) {
		fgets( recv_buf, MAX_BUF_LEN, stdin );
		if ( recv_buf[strlen(recv_buf) - 1] == '\n' ) {
			recv_buf[strlen(recv_buf) - 1] = '\0';
		}

		len = write_data( port_fd, recv_buf, strlen(recv_buf) + 1 );
		if ( len > 0 ) {
			fprintf( stdout, "%d send data successfully\n", i );
		} else {
			fprintf( stderr, "send data failed\n" );
		}
		sleep(2);
	}
	close_port(port_fd);

	return 0;
}
