/*
 * Serial Communications
 */

#ifndef __MY_SERIAL_H__
#define __MY_SERIAL_H__

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>    /* Unix 标准函数定义 */
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>   /* PPSIX 终端控制定义 */

/* 定义端口信息 */
typedef struct port_info {
	int baud_rate;
	int port_fd;
	char parity;
	char stop_bit;
	char flow_ctrl;
	char data_bits;
} *pport_info;

/* 打开/关闭串口 */
int open_port( char *port );
void close_port( int fd );

/* 根据波特率获得相应的波特率设置参数 */
int get_baud_rate( unsigned long int baud_rate );

/* 设置端口 */
int set_port( pport_info p_info );

/* 使用串口发送/接收数据 */
int send_data( int fd, char *data, int data_len );
int recv_data( int fd, char *data, int data_len );

#endif

/*
struct termios{
	unsigned short c_iflag; // 输入模式标志
	unsigned short c_oflag; //输出模式标志
    unsigned short c_cflag; // 控制模式标志
    unsigned short c_lflag; //区域模式标志或本地模式标志或局部模式
    unsigned char c_line;  //行控制line discipline 
    unsigned char c_cc[NCC]; // 控制字符特性
};
*/
