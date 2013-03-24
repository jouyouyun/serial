#include "parse_data.h"


int
main( int argc, char *argv[] )
{
	uint16_t crc1, crc2;
	//char str1[] = "026000000C018100FA02010115F202014A";
	//char str2[] = "011F0100163C0002026000000C018100FA02010115F202014A4CB9";
	char str1[] = "01600000180181FF680201011803000258048100BD050301FAF202014A";
	char str2[] = "011F0107163C000101600000180181FF680201011803000258048100BD050301FAF202014A67A7";

	debug("len: %d\t%d\n", strlen(str1), strlen(str2));
	crc1 = crc(str1, strlen(str1) / 2 );
	crc2 = crc(str2, strlen(str2) / 2 );
	debug( "crc1: %0X\n", crc1 );
	debug( "crcl: %0X\n", crc1 & 0XFF );
	debug( "crch: %0X\n", crc1 >> 8 );
	debug( "crc2: %0X\n", crc2 );
	debug( "crcl: %0X\n", crc2 & 0XFF );
	debug( "crch: %0X\n", crc2 >> 8 );

	return 0;
}
