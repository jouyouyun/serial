/* 处理数据帧 */

#include "parse_data.h"

/*
 * CRC 校验
 */

uint16_t
crc( unsigned char *p, int len )
{
	uint16_t ret = 0xFFFF;	/* 预置 16位 CRC 寄存器 */
	uint8_t i = 0;
	uint8_t k = 0;

	//debug("len: %d\n", len);
	for ( i = 0; i < len; ++i ) {
		//debug("p[%d] : %d\t%0uX\n", i, p[i], p[i]);
		ret ^= p[i];
		for ( k = 0; k < 8; ++k ) {
			/* 检查右移后的移出位 */
			ret = ( ret & 0x01) ? ( (ret >> 1) ^ 0xA001 ) : ( ret >> 1 );
		}
	}

	/* 将低 8 位与高 8 位相或 */
	ret = ((ret & 0x00FF) << 8) | ((ret & 0xFF00) >> 8);
	
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

uint16_t
strtoint(char *p, int len)
{
	uint8_t i;
	uint16_t num = 0;

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

uint64_t
hextoint(unsigned char *p, int len)
{
	uint8_t i;
	uint64_t num = 0;
	
	if ( p == NULL ) {
		debug("hextoint arguments err!\n");
		return 0;
	}

	for ( i = 0; i < len; ++i ) {
		num = (num << 8) + *(p + i);
	}
	//debug("num : %d\n", num);

	return num;
}

/*
 * 将字符串 2 个一组转换为16 进制
 */
void strtohex(const char *src, unsigned char *dest, int len)
{
	uint8_t i;
	
	if ( src == NULL ) {
		debug("strtohex arguments error!\n");
		return ;
	}
	
	memset(dest, 0, MAX_BUF_LEN);
	for ( i = 0; i < len / 2; ++i ) {
		dest[i] = strtoint((char*)(src + i * 2), 2);
		//debug("dest[%d] : %d\t%0X\n", i, dest[i], dest[i]);
	}
	
	return ;
}

/*
 * 处理数据帧
 */

void
parse_data ( data_frame *frame, unsigned char *strs, int len )
{
	uint8_t i, nums, tmp;
	uint8_t pos = 0;
	uint16_t crc1, crc2;

	if ( strs == NULL ) {
		debug("parse_data arguments err!\n");
		return;
	}

	memset( frame, 0, sizeof(data_frame));

	crc2 = crc(strs, len - 2 );
	//debug("crc2 : %d\t%0X\n", crc2, crc2);
	//debug("crc2l: %0X\tcrc2h: %0X\n", (crc2 >> 8), (crc2 & 0XFF));
	frame->crc2h = strs[len - 1];
	frame->crc2l = strs[len - 2];
	//debug("crc2l: %d\t%0X\n", frame->crc2l, frame->crc2l);
	//debug("crc2h: %d\t%0X\n", frame->crc2h, frame->crc2h);
	if ( ((crc2 >> 8) != frame->crc2l) || ((crc2 & 0XFF) != frame->crc2h) ) {
		debug( "CRC2 ERROR!\n" );
		return;
	}

	crc1 = crc((strs + 8), len - 8 - 4 );
	//debug("crc1 : %d\t%0X\n", crc1, crc1);
	//debug("crc1l: %0X\tcrc1h: %0X\n", (crc1 >> 8), (crc1 & 0XFF));
	frame->crc1h = strs[len - 3];
	frame->crc1l = strs[len - 4];
	//debug("crc1l: %d\t%0X\n", frame->crc1l, frame->crc1l);
	//debug("crc1h: %d\t%0X\n", frame->crc1h, frame->crc1h);
	if ( ((crc1 >> 8) != frame->crc1l) || ((crc1 & 0XFF) != frame->crc1h) ) {
		debug( "CRC1 ERROR!\n" );
		return;
	}
	
	//for ( i = 0; i < 8; ++i ) {
		//frame->node_addr[i] = strs[i];
		//debug("node_addr[%d] : %d\t%0X\n", i, frame->node_addr[i], frame->node_addr[i]);
	//}
	frame->node_addr = hextoint(strs, 8);
	//debug("node_addr : %d\t%0X\n", frame->node_addr, frame->node_addr);
	pos += 8;

	frame->dev_addr = strs[pos];
	//debug("dev_addr: %d\t%0X\n", frame->dev_addr, frame->dev_addr);
	pos++;
	
	frame->fun_code = strs[pos];
	//debug("fun_code: %d\t%0X\n", frame->fun_code, frame->fun_code);
	pos++;
	
	frame->start_addr = hextoint((strs + pos), 2);
	//debug("start_addr: %d\t%0X\n", frame->start_addr, frame->start_addr);
	pos += 2;

	frame->byte_nums = strs[pos];
	//debug("byte_nums: %d\t%0X\n", frame->byte_nums, frame->byte_nums);
	pos++;
	nums = frame->byte_nums / 4;	/* 数据组的个数 */
	for ( i = 0; i < nums; ++i ) {
		frame->datas[i].data_name = strs[pos];
		pos++;
		tmp = strs[pos] >> 4;	//高 4 位
		frame->datas[i].sign = (tmp & 0X8) >> 3;
		frame->datas[i].num_type = (tmp & 0X4) >> 2;
		frame->datas[i].long_int = (tmp & 0X2) >> 1;
		frame->datas[i].long_int_bit = (tmp & 0X1);
		
		frame->datas[i].decimal_point = strs[pos] & 0X7;	//低 4 位
		pos++;

		frame->datas[i].data_val = (strs[pos] << 8) + strs[pos + 1];
		//debug("data_val: %d\t%0X\n", frame->datas[i].data_val, frame->datas[i].data_val);
		pos += 2;
	}
}

void
print ( data_frame *frame )
{
	uint8_t i;

	if ( frame == NULL ) {
		debug( "print arguments err!\n" );
		return;
	}

	debug("\nnode_addr: %ld\t%0lX\n", frame->node_addr, frame->node_addr);
	
	debug( "dev_addr : %d\t%0X\n", frame->dev_addr, frame->dev_addr );
	debug("fun_code : %d\t%0X\n", frame->fun_code, frame->fun_code );
	debug("byte_nums: %d\t%0X\n", frame->byte_nums, frame->byte_nums);
	debug("start_addr: %d\t%0X\n", frame->start_addr, frame->start_addr );
	//debug("crc1l: %d\t%0X\n", frame->crc1l, frame->crc1l);
	//debug("crc1h: %d\t%0X\n", frame->crc1h, frame->crc1h);
	//debug("crc2l: %d\t%0X\n", frame->crc2l, frame->crc2l);
	//debug("crc2h: %d\t%0X\n", frame->crc2h, frame->crc2h);

	for ( i = 0; i < frame->byte_nums / 4; ++i ) {
		debug("\ndata_name: %d\n", frame->datas[i].data_name );
		debug("sign: %d\n", frame->datas[i].sign );
		debug("num_type: %d\n", frame->datas[i].num_type );
		debug("long_int: %d\n", frame->datas[i].long_int );
		debug("long_int_bit: %d\n", frame->datas[i].long_int_bit );
		debug("decimal_point: %d\n", frame->datas[i].decimal_point );
		debug("data_val: %d\n", frame->datas[i].data_val );
	}
	//debug("\ndata_val: %d\n", frame->datas[0].data_val - 65536 );
	//debug("data_val: %0.1f\n", (float)((frame->datas[0].data_val - 65536) * 0.1));
}
