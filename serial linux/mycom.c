/*本程序符合GPL条约
MyCom.c
*/
#include <stdio.h>              // printf
#include <fcntl.h>              // open
#include <string.h>             // bzero
#include <stdlib.h>             // exit
#include <sys/times.h>          // times
#include <sys/types.h>          // pid_t
#include <termios.h>		//termios, tcgetattr(), tcsetattr()
#include <unistd.h>
#include <sys/ioctl.h>          // ioctl
#include "mycom.h"

#define	TTY_DEV	"/dev/ttyS"	//端口路径

#define U16C550A		//for SBS pc104 serial port(cpu onboard com1\com2)  
//#define U16C954			//for SBS pc104+ serial port(sem/7513,scm/7026 com3-com6 etc.)

#define TIMEOUT_SEC(buflen,baud) (buflen*20/baud+2)  //接收超时
#define TIMEOUT_USEC 0
/*******************************************
 *	获得端口名称
********************************************/
static char *get_ptty(portinfo_t * pportinfo)
{
	char *ptty;

	switch(pportinfo->tty){
		case 0:{
			ptty = TTY_DEV"0";
		}break;
		case 1:{
			ptty = TTY_DEV"1";
		}break;
		case 2:{
			ptty = TTY_DEV"2";
		}break;
		case 3:{
			ptty = TTY_DEV"3";
		}break;
		case 4:{
			ptty = TTY_DEV"4";
		}break;
		case 5:{
			ptty = TTY_DEV"5";
		}break;
		case 6:{
			ptty = TTY_DEV"6";
		}break;
		case 7:{
			ptty = TTY_DEV"7";
		}break;
		case 8:{
			ptty = TTY_DEV"8";
		}break;
		case 9:{
			ptty = TTY_DEV"9";
		}break;
	}
	return(ptty);
}

/*******************************************
 *	波特率转换函数（请确认是否正确）
********************************************/
int convbaud(unsigned long int baudrate)
{
	switch(baudrate){
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
		default:
			return B9600;
	}
}

/*******************************************
 *	Setup comm attr
 *	fdcom: 串口文件描述符，pportinfo: 待设置的端口信息（请确认）
 *
********************************************/
int PortSet(int fdcom, const portinfo_t *pportinfo)
{
	struct termios termios_old, termios_new;
	int 	baudrate, tmp;
	char	databit, stopbit, parity, fctl;

	bzero(&termios_old, sizeof(termios_old));
	bzero(&termios_new, sizeof(termios_new));
	cfmakeraw(&termios_new);
	tcgetattr(fdcom, &termios_old);			//get the serial port attributions
	/*------------设置端口属性----------------*/
	//baudrates
	baudrate = convbaud(pportinfo -> baudrate);
	cfsetispeed(&termios_new, baudrate);		//填入串口输入端的波特率
	cfsetospeed(&termios_new, baudrate);		//填入串口输出端的波特率
	termios_new.c_cflag |= CLOCAL;			//控制模式，保证程序不会成为端口的占有者
	termios_new.c_cflag |= CREAD;			//控制模式，使能端口读取输入的数据

	// 控制模式，flow control
	fctl = pportinfo-> fctl;
	switch(fctl){
		case '0':{
			termios_new.c_cflag &= ~CRTSCTS;		//no flow control
		}break;
		case '1':{
			termios_new.c_cflag |= CRTSCTS;			//hardware flow control
		}break;
		case '2':{
			termios_new.c_iflag |= IXON | IXOFF |IXANY;	//software flow control
		}break;
	}

	//控制模式，data bits
	termios_new.c_cflag &= ~CSIZE;		//控制模式，屏蔽字符大小位
	databit = pportinfo -> databit;
	switch(databit){
		case '5':
			termios_new.c_cflag |= CS5;
		case '6':
			termios_new.c_cflag |= CS6;
		case '7':
			termios_new.c_cflag |= CS7;
		default:
			termios_new.c_cflag |= CS8;
	}

	//控制模式 parity check
	parity = pportinfo -> parity;
	switch(parity){
		case '0':{
			termios_new.c_cflag &= ~PARENB;		//no parity check
		}break;
		case '1':{
			termios_new.c_cflag |= PARENB;		//odd check
			termios_new.c_cflag &= ~PARODD;
		}break;
		case '2':{
			termios_new.c_cflag |= PARENB;		//even check
			termios_new.c_cflag |= PARODD;
		}break;
	}

	//控制模式，stop bits
	stopbit = pportinfo -> stopbit;
	if(stopbit == '2'){
		termios_new.c_cflag |= CSTOPB;	//2 stop bits
	}
	else{
		termios_new.c_cflag &= ~CSTOPB;	//1 stop bits
	}

	//other attributions default
	termios_new.c_oflag &= ~OPOST;			//输出模式，原始数据输出
	termios_new.c_cc[VMIN]  = 1;			//控制字符, 所要读取字符的最小数量
	termios_new.c_cc[VTIME] = 1;			//控制字符, 读取第一个字符的等待时间	unit: (1/10)second

	tcflush(fdcom, TCIFLUSH);				//溢出的数据可以接收，但不读
	tmp = tcsetattr(fdcom, TCSANOW, &termios_new);	//设置新属性，TCSANOW：所有改变立即生效	tcgetattr(fdcom, &termios_old);
	return(tmp);
}

/*******************************************
 *	Open serial port
 *	tty: 端口号 ttyS0, ttyS1, ....
 *	返回值为串口文件描述符
********************************************/
int PortOpen(portinfo_t *pportinfo)
{
	int fdcom;	//串口文件描述符
	const char *ptty;

	ptty = (const char *)get_ptty(pportinfo);
	//fdcom = open(ptty, O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY);
	fdcom = open(ptty, O_RDWR | O_NOCTTY | O_NONBLOCK);

	return (fdcom);
}

/*******************************************
 *	Close serial port
********************************************/
void PortClose(int fdcom)
{
	close(fdcom);
}

/********************************************
 *	send data
 *	fdcom: 串口描述符，data: 待发送数据，datalen: 数据长度
 *	返回实际发送长度
*********************************************/
int PortSend(int fdcom, char *data, int datalen)
{
	int len = 0;
#ifdef U16C954
	int line =TIOCM_DTR;			//for 16954 chip,use DTR instead of RTS
#else
	int line =TIOCM_RTS;
#endif
	if(ioctl(fdcom,TIOCMBIS,&line))		// 1--send 0--receive
		printf("enter send mode error \n");
	usleep(10);
	
	len = write(fdcom, data, datalen);	//实际写入的长度
	if(len == datalen){
		return (len);
	}
	else{
		tcflush(fdcom, TCOFLUSH);
		return -1;
	}
}

/*******************************************
 *	receive data
 *	返回实际读入的字节数
 *
********************************************/
int PortRecv(int fdcom, char *data, int datalen, int baudrate)
{
	int readlen, fs_sel;
	fd_set	fs_read;
	struct timeval tv_timeout;
#ifdef U16C954
	int line = TIOCM_DTR;
#else
	int line = TIOCM_RTS;
#endif
	
	if (ioctl(fdcom,TIOCMBIC,&line))
		printf("enter receive mode error\n");
	usleep(10);

	FD_ZERO(&fs_read);
	FD_SET(fdcom, &fs_read);
	tv_timeout.tv_sec = TIMEOUT_SEC(datalen, baudrate);
	tv_timeout.tv_usec = TIMEOUT_USEC;

	fs_sel = select(fdcom+1, &fs_read, NULL, NULL, &tv_timeout);
	if(fs_sel){
		readlen = read(fdcom, data, datalen);
		return(readlen);
	}
	else{
		return(-1);
	}

	return (readlen);
}

//*************************Test*********************************
int main(int argc, char *argv[])
{
	int fdcom, i, SendLen, RecvLen;
	struct termios termios_cur;
	char RecvBuf[10];
	portinfo_t portinfo ={
		'0',                          	// print prompt after receiving
 		19200,                      	// baudrate: 115200
 		'8',                          	// databit: 8
 		'0',                          	// debug: off
 		'0',                          	// echo: off
 		'0',                          	// flow control: none
 		atoi(argv[2]),                  // 获取第二参数决定使用哪个串口
 		'0',                          	// parity: none
 		'1',                          	// stopbit: 1
 		 0    	                  	// reserved
	};

	if(argc != 3){
		printf("Usage: %s type port\n",argv[0]);
		printf("  type section may be: 0--send,1--receive\n");
		printf("  port section may be: 0~9\n");
		exit(-1);
	}

	fdcom = PortOpen(&portinfo);
	if(fdcom<0){
		printf("Error: open serial port error.\n");
		exit(1);
	}

	PortSet(fdcom, &portinfo);

	if(atoi(argv[1]) == 0){
		//send data
		for(i=0; i<100; i++){
			SendLen = PortSend(fdcom, "1234567890", 10);
			if(SendLen>0){
				printf("No %d send %d data 1234567890.\n", i, SendLen);
			}
			else{
				printf("Error: send failed.\n");
			}
			sleep(1);
		}
		PortClose(fdcom);
	}
	else{
		for(;;){
			RecvLen = PortRecv(fdcom, RecvBuf, 10, portinfo.baudrate);
			if(RecvLen>0){
				for(i=0; i<RecvLen; i++){
					printf("Receive data No %d is %x.\n", i, RecvBuf[i]);
				}
				printf("Total frame length is %d.\n", RecvLen);
			}
			else{
				printf("Error: receive error.\n");
			}
			sleep(2);
		}
	}
	return 0;
}
