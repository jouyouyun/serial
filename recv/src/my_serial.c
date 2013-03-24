/*
 * 串口读写程序
 */

#include "my_serial.h"
#include "parse_data.h"

/* 打开串口 */
int open_port( char *port )
{
	int fd;

	if ( (fd = open( port, O_RDWR | O_NOCTTY | O_NONBLOCK )) == -1 ) {
		debug( "Cannot open the port: %s\n", strerror(errno) );
		return -1;
	}

	return fd;
}

/* 关闭串口 */
void close_port()
{
	if ( port_fd > 2 ) {
		close(port_fd);
	}
}

/* 使用串口发送数据 */
int write_data( int fd, char *data, int data_len )
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
int read_data( int fd, char *data, int data_len )
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

void recv_msg(pport_info pinfo)
{
	int i, j, ret;
	int len = 0;
	char buf[MAX_BUF_LEN];
	pthread_t thrd;
	
	j = 0;
	/* 接受数据 */
	while (1) {
		memset(buf, 0, MAX_BUF_LEN);
		len = read_data( pinfo->port_fd, buf, MAX_BUF_LEN );
		if ( len > 0 ) {
			for ( i = 0; i < len; i++ ) {
				recv[j] = buf[i] & 0XFF;
				j++;
			}
			
			//cnt++;
			if ( j > 20 ) {
				fprintf(stdout, "recv: ");
				for ( i = 0; i < j; i++ ) {
					fprintf(stdout, "%0X ", recv[i] );
				}
				fprintf(stdout, "\n");
				//fprintf(stdout, "\nlen : %d\n", j);
				
				ret = pthread_create(&thrd, NULL, parse_data, &j );
				if ( ret != 0 ) {
				debug("create thrd err: %s\n", strerror(errno) );
				}
				usleep(100);
				pthread_detach(thrd);
				memset(recv, 0, MAX_BUF_LEN);
				j = 0;
				//cnt = 0;
			}
		} else {
			continue;
		}
		sleep(1);
	}
}
