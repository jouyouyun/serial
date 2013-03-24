/* 处理数据帧 */

#include "parse_data.h"

/*
 * CRC 校验
 */

uint16_t
crc( uint8_t *p, int len )
{
	uint16_t ret = 0xFFFF;	/* 预置 16位 CRC 寄存器 */
	int i = 0;
	int k = 0;

	debug("\np: %s\nlen: %d\n", p, len);
	for ( i = 0; i < len; ++i ) {
		ret ^= p[i];
		for ( k = 0; k < 8; ++k ) {
			/* 检查右移后的移出位 */
			ret = ( ret & 0x01) ? ( (ret >> 1) ^ 0xA001 ) : ( ret >> 1 );
		}
	}

	/* 将低 8 位与高 8 位相或 */
	ret = ((ret & 0x00FF) << 8) | ((ret & 0xFF00) >> 8);
	//ret = ((ret << 8) | (ret >> 8)) & 0xFFFF;
	debug("ret: %d\t%0X\n", ret);
	return ret;
}

int
chtoint( char c )
{
	if ( c >= '0' && c <= '9' ) {
		return ( c - '0' );
	} else if ( c >= 'a' && c <= 'z' ) {
		return ( c - 'a' + 10 );
	}

	return ( c - 'A' + 10 );
}

uint64_t
strtoint(char *p, int len)
{
	int i;
	uint64_t num = 0;

	if ( p == NULL ) {
		debug("strtoint arguments err!\n");
		return 0;
	}

	for ( i = 0; i < len; ++i ) {
		num = num * 16 + chtoint(*(p + i));
	}
	//debug("num : %d\n", num);

	return num;
}

/*
 * 将字符串 2 个一组转换为16 进制
 */
void strtohex(const char *src, uint8_t *dest)
{
	int i;
	int len;
	
	if ( src == NULL ) {
		debug("strtohex arguments error!\n");
		return ;
	}
	
	len = strlen(src);
	for ( i = 0; i < len / 2; ++i ) {
		dest[i] = strtoint((src + i * 2), 2);
	}
	
	return ;
}

/*
 * 处理数据帧
 */

void
parse_data ( data_frame *frame, char *strs )
{
	int i, len, nums, tmp;
	int pos = 0;
	uint16_t crc1, crc2;

	if ( strs == NULL ) {
		debug("parse_data arguments err!\n");
		return;
	}

	memset( frame, 0, sizeof(data_frame));
	len = strlen(strs);

	crc2 = crc((uint8_t*)strs, len / 2 - 2 );
	frame->crc2h = strtoint((strs + len - 2), 2);
	frame->crc2l = strtoint((strs + len - 4), 2);
	if ( ((crc2 & 0XFF) != frame->crc2l) || ((crc2 >> 8) != frame->crc2h) ) {
		debug( "CRC2 ERROR!\n" );
		//return;
	}

	crc1 = crc((uint8_t*)(strs + 16), len / 2 - 8 - 4 );
	frame->crc1h = strtoint((strs + len - 6), 2);
	frame->crc1l = strtoint((strs + len - 8), 2);
	if ( ((crc1 & 0XFF) != frame->crc1l) || ((crc1 >> 8) != frame->crc1h) ) {
		debug( "CRC1 ERROR!\n" );
		//return;
	}
	debug("crc1l: %d\t%0X\n", frame->crc1l, frame->crc1l);
	debug("crc1h: %d\t%0X\n", frame->crc1h, frame->crc1h);
	debug("crc2l: %d\t%0X\n", frame->crc2l, frame->crc2l);
	debug("crc2h: %d\t%0X\n", frame->crc2h, frame->crc2h);
	
	for ( i = 0; i < 8; ++i ) {
		//frame->node_addr[i] = chtoint(strs[i * 2]) * 16 + chtoint(strs[i * 2 + 1]);
		frame->node_addr[i] = strtoint((strs + 2 * i), 2);
		debug("node_addr[%d] : %d\t%0X\n", i, frame->node_addr[i], frame->node_addr[i]);
	}
	//frame->node_addr = strtoint(strs, 16);
	//debug("node_addr : %d\t%0X\n", frame->node_addr, frame->node_addr);
	pos += 16;

	//frame->dev_addr = chtoint(strs[pos]) * 16 + chtoint(strs[pos+1]);
	frame->dev_addr = strtoint((strs + pos), 2);
	debug("dev_addr: %d\t%0X\n", frame->dev_addr, frame->dev_addr);
	pos += 2;
	//frame->fun_code = chtoint(strs[pos]) * 16 + chtoint(strs[pos+1]);
	frame->fun_code = strtoint((strs + pos), 2);
	debug("fun_code: %d\t%0X\n", frame->fun_code, frame->fun_code);
	pos += 2;
	/*
	for ( i = 0; i < 2; ++i ) {
		frame->start_add[i] = chtoint(strs[pos]) * 16 + chtoint(strs[pos+1]);
		pos += 2;
	}*/
	frame->start_addr = strtoint((strs + pos), 4);
	debug("start_addr: %d\t%0X\n", frame->start_addr, frame->start_addr);
	pos += 4;

	//frame->byte_nums = chtoint(strs[pos]) * 16 + chtoint(strs[pos+1]);
	frame->byte_nums = strtoint((strs + pos), 2);
	debug("byte_nums: %d\t%0X\n", frame->byte_nums, frame->byte_nums);
	pos += 2;
	nums = frame->byte_nums / 4;	/* 数据组的个数 */
	for ( i = 0; i < nums; ++i ) {
		frame->datas[i].data_name = strtoint((strs + pos), 2);
		pos += 2;
		tmp = chtoint(strs[pos]);
		frame->datas[i].sign = (tmp & 0X8) >> 3;
		frame->datas[i].num_type = (tmp & 0X4) >> 2;
		frame->datas[i].long_int = (tmp & 0X2) >> 1;
		frame->datas[i].long_int_bit = (tmp & 0X1);
		pos++;
		frame->datas[i].decimal_point = chtoint(strs[pos]);
		pos++;

		frame->datas[i].data_val = strtoint(strs + pos, 4);
		debug("data_val: %d\t%0X\n", frame->datas[i].data_val, frame->datas[i].data_val);
		pos += 4;
	}
}

void
print ( data_frame *frame )
{
	int i;

	if ( frame == NULL ) {
		debug( "print arguments err!\n" );
		return;
	}

	debug("node_addr: %d\t%0X\n", frame->node_addr, frame->node_addr );
	debug( "dev_addr : %d\t%0X\n", frame->dev_addr, frame->dev_addr );
	debug("fun_code : %d\t%0X\n", frame->fun_code, frame->fun_code );
	debug("byte_nums: %d\t%0X\n", frame->byte_nums, frame->byte_nums);
	debug("start_addr: %d\t%0X\n", frame->start_addr, frame->start_addr );
	debug("crc1l: %d\t%0X\n", frame->crc1l, frame->crc1l);
	debug("crc1h: %d\t%0X\n", frame->crc1h, frame->crc1h);
	debug("crc2l: %d\t%0X\n", frame->crc2l, frame->crc2l);
	debug("crc2h: %d\t%0X\n", frame->crc2h, frame->crc2h);

	for ( i = 0; i < frame->byte_nums / 4; ++i ) {
		debug("data_name: %d\n", frame->datas[i].data_name );
		debug("sign: %d\n", frame->datas[i].sign );
		debug("num_type: %d\n", frame->datas[i].num_type );
		debug("long_int: %d\n", frame->datas[i].long_int );
		debug("long_int_bit: %d\n", frame->datas[i].long_int_bit );
		debug("decimal_point: %d\n", frame->datas[i].decimal_point );
		debug("data_val: %d\n", frame->datas[i].data_val );
	}
}
