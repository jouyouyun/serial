/*本程序符合GPL条约
MyCom.h 
一组操作串口的函数
*/

//串口结构
typedef struct{
	char	prompt;		//prompt after reciving data
	int 	baudrate;		//baudrate
	char	databit;		//data bits, 5, 6, 7, 8
	char 	debug;		//debug mode, 0: none, 1: debug
	char 	echo;			//echo mode, 0: none, 1: echo
	char	fctl;			//flow control, 0: none, 1: hardware, 2: software
	char 	tty;			//tty: 0, 1, 2, 3, 4, 5, 6, 7
	char	parity;		//parity 0: none, 1: odd, 2: even
	char	stopbit;		//stop bits, 1, 2
	const int reserved;	//reserved, must be zero
}portinfo_t;
portinfo_t *pportinfo;
/*
 *	打开串口，返回文件描述符
 *	pportinfo: 待设置的串口信息
*/
int PortOpen(portinfo_t *pportinfo);
/*
 *	设置串口
 *	fdcom: 串口文件描述符， pportinfo： 待设置的串口信息
*/
int PortSet(int fdcom, const portinfo_t *pportinfo);
/*
 *	关闭串口
 *	fdcom：串口文件描述符
*/
void PortClose(int fdcom);
/*
 *	发送数据
 *	fdcom：串口描述符， data：待发送数据， datalen：数据长度
 *	返回实际发送长度
*/
int PortSend(int fdcom, char *data, int datalen);
/*
 *	接收数据
 *	fdcom：串口描述符， data：接收缓冲区, datalen.：接收长度， baudrate：波特率
 *	返回实际读入的长度
*/
int PortRecv(int fdcom, char *data, int datalen, int baudrate);
