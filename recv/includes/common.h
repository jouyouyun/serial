/*
 * 
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>		/* 定义了 uint8_t int8_t ... */

#define debug(args ...) fprintf(stderr, args)
/* 数据缓冲区 */
#define MAX_BUF_LEN 256
#define MAX_TIME_LEN 30
#define MAX_NAME_LEN 20

/* 定义串口信息 */
typedef struct port_info {
	int baud_rate;
	int port_fd;
	char parity;
	char stop_bit;
	char flow_ctrl;
	char data_bits;
} *pport_info;

/* 定义数据组名称、格式及数值 */
typedef struct data_type {
	uint8_t data_name;		/* 数据组名称 */

	/* 定义数据格式 */
	uint8_t sign : 1;			/* 0 无符号数，1 有符号数 */
	uint8_t num_type : 1;		/* 数值类型，0 数值，1 开关量 */
	uint8_t long_int : 1;		/* 长整型数值标识，0 双字节，1 四字节 */
	uint8_t long_int_bit : 1;	/* 四字节数标识，0 低 2 字节，1 高 2 字节 */
	uint8_t empty_bit : 1;		/* 保留位 */
	uint8_t decimal_point : 3;	/* 小数点位：0 - 7，最多 7 位小数 */

	uint16_t data_val;		/* 数据组的值高 8 位，低 8 位 */
} data_type;

/* 定义帧 */
typedef struct data_frame {
	//uint8_t node_addr[8];	/* 节点地址 */
	uint64_t node_addr;		/* 节点地址 */
	
	uint8_t dev_addr;		/* 设备地址 */
	uint8_t fun_code;		/* 功能码 */
	uint16_t start_addr;	/* 起始地址 */
	
	uint8_t byte_nums;		/* 字节计数 */
	uint8_t empty_bit[3];	/* 保留位 */

	data_type datas[MAX_BUF_LEN];	/* 数据组 */

	uint8_t crc1l, crc1h;	/* CRC1 的高 8 位，低 8 位 */
	uint8_t crc2l, crc2h;	/* CRC2 的高 8 位，低 8 位 */
}data_frame;

typedef union data_value {
	int16_t  num;	/* 2 字节整形 */
	//uint16_t unum;	/* 2 字节无符号整形 */
	float fnum;		/* 有小数 */
} data_value;

typedef struct res_arg {
	char name[MAX_NAME_LEN];
	data_value vaule;
} res_arg;

/* 保存结果 */
typedef struct results {
	uint64_t node_addr;		/* 节点地址 */
	char cur_time[MAX_TIME_LEN];
	res_arg args[MAX_BUF_LEN];
} results;

#endif
