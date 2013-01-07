/*
 * 模拟 Modbus 协议
 */ 

#ifndef __MODBUS_H__
#define __MODBUS_H__

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define MAX_RECV_LEN 65

unsigned short modbus_crc( unsigned char *p, int len );

#endif
