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
#include <termios.h>   /* PPSIX 终端控制定义 */
#include <mysql/mysql.h>

/* 数据缓冲区 */
#define MAX_BUF_LEN 256

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
int send_data( int fd, char *data, int data_len );
int recv_data( int fd, char *data, int data_len );
//end

/* 字符串与数字相互转换 */
long int str_to_int( char *str );
void str_delim( const char *str, const char *delim );
//end

/* db_switch */
/* 数据库 */
#define db_name "db_serial"
#define MAX_NAME_LEN 20
char table_name[MAX_NAME_LEN];

void init_mysql( MYSQL *pdata );
void get_table_name();
int get_cur_time( char *str_time, int type );
void *db_switch_thread( void *arg );
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
