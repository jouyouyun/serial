/*
 * Modbus CRC
 * 算法思想：
 * 1.预置 1 个 16 位的寄存器为 0xFFFF, 称此寄存器为 CRC 寄存器
 * 2.把第一个 8 位二进制数据（即通信信息帧的第一个字节）与 16 
 *   位的 CRC 寄存器的低 8 位相异或，把结果放回 CRC 寄存器
 * 3.把 CRC 寄存器的内容右移一位，用 0 填补最高位，并检查右移后的移出位
 * 4.如果移出位为 0,重复第 3 步；如果为 1, CRC 寄存器与 0xA001 进行异或
 * 5.重复 3 和 4，直到右移 8 次，这样整个 8 位数据就全部进行了处理
 * 6.重复 2 到 5 步，进行通信信息帧下一个字节的处理
 * 7.完成上述步骤后，得到的 16 位 CRC 寄存器的高、低字节已经交换
 * 8.最后得到的 CRC 寄存器内容即为 CRC 码
 *
 */

#include "modbus.h"

unsigned short modbus_crc( unsigned char *p, int len )
{
	unsigned short ret = 0xFFFF;	/* 预置 16位 CRC 寄存器 */
	int i = 0;
	int k = 0;

	for ( i = 0; i < len; ++i ) {
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

/*
int main( int argc, char *argv[] )
{
	unsigned char input[] = { 0x01, 0x04, 0x0D, 0x28, 0x00, 0x02, 0x01, 
	0x02, 0x00, 0x53, 0x00, 0x01 };
	unsigned short out = 0;
	int i = 0;
	int len = sizeof(input);

	out = modbus_crc( input, sizeof(input) );
	
	printf( "Input:" );
	for ( i = 0; i < len; i++ ) {
		printf( " 0x%.2X", input[i] );
	}
	printf( "\nModbus CRC is: 0x%.4X\n", out );

	return 0;
}
*/
