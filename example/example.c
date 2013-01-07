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

/* 根据波特率获得相应的波特率设置参数 */
int get_baud_rate( unsigned long int baud_rate )
{
	switch( baud_rate ) {
		case 0:
			return B0;
		case 50:
			return B50;
		case 75:
			return B75;
		case 110:
			return B110;
		case 134:
			return B134;
		case 150:
			return B150;
		case 200:
			return B200;
		case 300:
			return B300;
		case 600:
			return B600;
		case 1200:
			return B1200;
		case 1800:
			return B1800;
		case 2400:
			return B2400;
		case 4800:
			return B4800;
		case 9600:
			return B9600;
		case 19200:
			return B19200;
		case 38400:
			return B38400;
		case 57600:
			return B57600;
		case 115200:
			return B115200;
		case 230400:
			return B230400;
		default:
			return -1;
	}
}

/* 设置端口 */
int set_port( pport_info p_info )
{
	struct termios old_opt, new_opt;
	int baud_rate;
	int ret;

	memset( &old_opt, 0, sizeof(struct termios) );
	memset( &new_opt, 0, sizeof(struct termios) );

	cfmakeraw( &new_opt );
	tcgetattr( p_info->port_fd, &old_opt );

	/*============一下设置波特率=========*/
	baud_rate = get_baud_rate(p_info->baud_rate);

	/* 修改 new_opt 结构体中的串口输入/输出波特率参数 */
	cfsetispeed( &new_opt, baud_rate );
	cfsetospeed( &new_opt, baud_rate );

	/* 修改控制模式，保证程序不回占用串口 */
	new_opt.c_cflag |= CLOCAL;
	/* 修改控制模式，使得能够从串口中读取输入数据 */
	new_opt.c_cflag |= CREAD;

	/*============设置数据流控制=======*/
	switch (p_info->flow_ctrl) {
		case '0':
			// 不使用流控制
			new_opt.c_cflag &= CRTSCTS;
			break;
		case '1':
			// 使用硬件进行流控制
			new_opt.c_cflag |= CRTSCTS;
			break;
		case '2':
			new_opt.c_cflag |= IXON | IXOFF | IXANY;
			break;
		default :
			break;
	}

	/*============设置数据位=========*/
	new_opt.c_cflag &= ~CSIZE;
	switch (p_info->data_bits) {
		case '5':
			new_opt.c_cflag |= CS5;
			break;
		case '6':
			new_opt.c_cflag |= CS6;
			break;
		case '7':
			new_opt.c_cflag |= CS7;
			break;
		case '8':
			new_opt.c_cflag |= CS8;
			break;
		default :
			new_opt.c_cflag |= CS8;
			break;
	}

	/*============设置奇偶校验位========*/
	switch(p_info->parity) {
		case '0':
			// 不使用奇偶校验
			new_opt.c_cflag |= ~PARENB;
			break;
		case '1':
			// 使用偶校验
			new_opt.c_cflag |= PARENB;
			new_opt.c_cflag |= ~PARODD;
			break;
		case '2':
			// 使用奇校验
			new_opt.c_cflag |= PARENB;
			new_opt.c_cflag |= PARODD;
			break;
		default :
			break;
	}

	/*============设置停止位========*/
	if ( p_info->stop_bit == '2' ) {
		new_opt.c_cflag |= CSTOPB;
	} else {
		new_opt.c_cflag &= ~CSTOPB;
	}

	/* 修改输出模式，原始数据输出 */
	new_opt.c_cflag &= ~OPOST;
	/* 修改控制字符，读取字符的个数最少为 1 */
	new_opt.c_cc[VMIN] = 1;
	/* 修改控制字符，读取第一个字符等待 1 * (1 / 10) s */
	new_opt.c_cc[VTIME] = 1;

	/* 如果发生数据溢出，接收数据，但不再读取 */
	tcflush( p_info->port_fd, TCIFLUSH );

	ret = tcsetattr( p_info->port_fd, TCSANOW, &new_opt );
	if ( ret == -1 ) {
		fprintf( stderr, "Set port parameters: %s\n", strerror(errno) );
		return -1;
	}

	tcgetattr( p_info->port_fd, &old_opt );

	return ret;
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
