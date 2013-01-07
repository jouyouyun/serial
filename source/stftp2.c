/* 文件名: stftp.c */
/*   说明: 简单的TFTP 服务器例程 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>


/* TFTP 协议操作码定义 */
#define  RRQ     1   /* 读数据请求 */ 
#define  WRQ     2   /* 写数据请求*/
#define  DATA    3   /* 数据块 */
#define  ACK     4   /* 确认 */
#define  ERROR   5   /* 发生错误 */

/* TFTP 协议错误码定义 */
#define  FILE_NOT_FOUND  1  /* 文件未找到 */
#define  BAD_OP          4  /* 非法操作 */


/* 显示错误信息并退出 */
void die(char *msg)
{
	perror(msg);
	exit(1);
}

/* TFTP 协议数据包格式*/
struct tftp_packet
{
	short opcode;  /* 2字节操作码 */
	union
	{
		char bytes[514];  /* 最多514个字节，2字节块号 + 512字节数据 */
		struct 
		{
			short code;          /* 错误码 */
			char message[200];   /* 错误信息 */
		}error; 		     /* 错误包 */
		struct
		{
			short block;         /* 块号*/
			char bytes[512];     /* 文件数据 */
		}data;			     /* 数据包 */
		struct 
		{
			short block;	     /* 块号 */
		}ack;			     /* 确认包 */
	}u;
};


/* 发送错误包 */
void send_error(int s, struct sockaddr_in *addr,socklen_t addr_len, int code)
{
	struct tftp_packet err;
	int size, bytes;

	err.opcode = htons(ERROR);
	err.u.error.code = htons(code);
	switch(code)
	{
		case FILE_NOT_FOUND:
			strcpy(err.u.error.message, "file not found");
			break;
		case BAD_OP:
			strcpy(err.u.error.message, "bad op");
                        break;
		default:
			strcpy(err.u.error.message, "undefined");
	}

	/* 2字节操作码 + 2字节错误码 + 错误信息 + 字符串结束符 */
	size = 2 + 2 + strlen(err.u.error.message) + 1;
	bytes = sendto(s, &err, size, 0, (struct sockaddr *)addr, addr_len);
	if(bytes != size)
		perror("send error : sendto");
}

void handle_request(int sock, struct sockaddr_in *from, socklen_t from_len, struct tftp_packet *request)
{
	char *filename;
	char *mode;
	int fd;
	int size;
	int bytes;
	struct tftp_packet data, response;
	int block;
	struct sockaddr_in new_from;
	socklen_t new_from_len;

	/* 分析操作码 */
	request->opcode = ntohs(request->opcode);
	if(request->opcode != RRQ)
		return ;
	filename = request->u.bytes;
	mode = filename + strlen(filename) + 1;  /* 得到传输模式  此处有些不明白 */
	printf("Reqested filename is %s, mode is %s\n",filename, mode);

	/* 在此支持 octet 模式 */
	if(strcmp(mode, "octet"))
	{
		send_error(sock,from, from_len, BAD_OP);
		return ;
	}
	
	fd = open(filename, O_RDONLY);
	if(fd < 0)
	{
		send_error(sock, from, from_len, FILE_NOT_FOUND);
		return ;
	}
	
	printf("Sending start!\n");
	data.opcode = htons(DATA);
	for(block = 1; (size = read(fd, data.u.data.bytes, 512)) > 0; block++)
	{
		data.u.data.block = block;
		/* 加上2字节操作码，2字节块号的长度 */
		size += 4;
		bytes = sendto(sock, &data, size, 0, (struct sockaddr *)from, from_len);
		if(bytes != size)
		{
			perror("handle_request : sendto error");
			close(fd);
			return;
		}

		/* 接受确认 */
		new_from_len = sizeof(new_from);
		bytes = recvfrom(sock, &response, sizeof(response), 0, (struct sockaddr *)&new_from, &new_from_len);
		if(bytes < 0)
		{
			perror("handle_request : recvfrom error");
			close(fd);
			return;
		}

		/* 如果数据包非来自原来的客户端，说明受到干扰 */
		if(new_from.sin_addr.s_addr != from->sin_addr.s_addr)
		{
			fprintf(stderr, "handle_request : bad address\n");
			close(fd);
			return;
		}

		response.opcode = ntohs(response.opcode);
		if(response.opcode != ACK)
		{
			fprintf(stderr, "handle_request : not ACK\n");
			close(fd);
			return;
		}

		/* 判断确认的块号是否正确 */
		response.u.ack.block = ntohs(response.u.ack.block);
		if(response.u.ack.block != block)
		{
			fprintf(stderr, "handle_request : wrong block!\n");
			close(fd);
			return;
		}
		printf(".");  /* 输出信息表示一块数据传输成功 */
	}
	printf("\nSent out.\n");
}


int main()
{
	int s, bytes, val;
	struct sockaddr_in addr,from;
	socklen_t from_len;
	struct tftp_packet packet;

	if((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		die("socket error");
	val = 1;
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
	memset(&addr, 0 ,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(1237);

	if(bind(s, (struct sockaddr *)&addr, sizeof(addr)))
		die("bind error");
	
	while(1)
	{
		from_len = sizeof(from);
		bytes = recvfrom(s, &packet, sizeof(packet), 0, (struct sockaddr *)&from, &from_len);
		if(bytes < 0)
			die("recvfrom error");
		printf("Request from %s : %d\n", inet_ntoa(from.sin_addr), htons(from.sin_port));
	handle_request(s, &from, from_len, &packet);
	}
}
