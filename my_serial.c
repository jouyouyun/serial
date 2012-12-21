/*
 * 串口读写程序
 */

#include "my_serial.h"

/* 打开串口 */
int open_port( char *port )
{
	int fd;

	if ( (fd = open( port, O_RDWR | O_NOCTTY | O_NONBLOCK )) == -1 ) {
		fprintf( stderr, "Cannot open the port: %s\n", strerror(errno) );
		return -1;
	}

	return fd;
}

/* 关闭串口 */
void close_port( int fd )
{
	close(fd); 
}

/* 使用串口发送数据 */
int send_data( int fd, char *data, int data_len )
{
	int len = 0;

	len = write( fd, data, data_len );
	if ( len == data_len ) {
		return len;
	} else {
		/* 如果出现溢出 */
		tcflush(fd, TCOFLUSH );
		return -1;
	}
}

/* 使用串口接收数据 */
int recv_data( int fd, char *data, int data_len )
{
	int len, fs_sel;
	fd_set fs_read;
	struct timeval tv;

	FD_ZERO( &fs_read );
	FD_SET( fd, &fs_read );

	tv.tv_sec = 10;
	tv.tv_usec = 0;

	/* 使用 select 实现串口的多路通信 */
	fs_sel = select( fd + 1, &fs_read, NULL, NULL, &tv );
	if ( fs_sel ) {
		len = read( fd, data, data_len );
		return len;
	} else {
		return -1;
	}
}
