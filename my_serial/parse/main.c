#include "parse_data.h"

int
main ( int argc, char *argv[] )
{
	//int i;
	//uint16_t crc1, crc2;
	int len;
	data_frame f;
	char str[MAX_BUF_LEN];
	unsigned char recv[MAX_BUF_LEN];

	printf ( "Input:\n" );
	fgets( str, MAX_BUF_LEN, stdin );
	if ( str[strlen(str) - 1] == '\n' ) {
		str[strlen(str) - 1] = '\0';
	}

	len = strlen(str);
	debug("len : %d\n", len);
	strtohex(str, recv, len);
	//crc1 = crc(recv, len / 2 - 2);
	//debug("crc1: %d\t%0X\n", crc1, crc1);
	//crc2 = crc(recv + 8, len / 2 - 8 - 4);
	//debug("crc2: %d\t%0X\n", crc2, crc2);
	parse_data( &f, recv, len / 2 );
	print( &f );

	return 0;
}
