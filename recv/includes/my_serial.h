/*
 * Serial Communications
 */

#ifndef __MY_SERIAL_H__
#define __MY_SERIAL_H__

#include "common.h"
#include <unistd.h>    /* Unix 标准函数定义 */
#include <fcntl.h>
#include <termios.h>   /* PPSIX 终端控制定义 */

int port_fd;

/* 打开/关闭串口 */
int open_port( char *port );
void close_port();
//end

/* 设置端口 */
int set_port( pport_info p_info );
/* 根据波特率获得相应的波特率设置参数 */
int get_baud_rate( unsigned long int baud_rate );
/*============设置数据流控制=======*/
int set_flow_ctrl( pport_info p_info, struct termios *opt );
/*============设置数据位=========*/
int set_data_bit( pport_info p_info, struct termios *opt );
/*============设置奇偶校验位========*/
int set_parity( pport_info p_info, struct termios *opt );
/*============设置停止位========*/
int set_stop_bit( pport_info p_info, struct termios *opt );
//end set_port

/* 使用串口发送/接收数据 */
int write_data( int fd, char *data, int data_len );
int read_data( int fd, char *data, int data_len );
//end

void recv_msg(pport_info pinfo);

/* 字符串与数字相互转换 */
long int str_to_int( char *str );
void str_delim( const char *str, const char *delim );
//end

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
