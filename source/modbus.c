/*
 * 模拟 Modbus 协议
 */

#include "modbus.h"

int chartoint( char c )
{
	if ( c >= '0' && c <= '9' ) {
		return ( c - '0' );
	}

	return ( c - 'A' + 10 );
}

int main( int argc, char *argv[] )
{
	int saveadd = 0, func = 0, startadd = 0, num = 0, i = 0, len = 0, k = 0;
	unsigned short val = 0;
	unsigned char recv[MAX_RECV_LEN];
	unsigned char recvtmp[MAX_RECV_LEN];
	unsigned char str[MAX_RECV_LEN];
	unsigned char fstr[4];
	float *pf = NULL;

	while ( scanf( "%d, %d, %d, %d", &saveadd, &func, &startadd, &num ) 
			!= EOF ) {
		str[0] = saveadd;
		str[1] = func;
		str[3] = startadd & 0xFF;
		startadd >>= 8;
		str[2] = startadd & 0xFF;
		str[5] = num & 0xFF;
		num >>= 8;
		str[4] = num & 0xFF;
		val = modbus_crc( str, 6 );
		//printf( "%02X\t", val );
		str[7] = val & 0xFF;
		val >>= 8;
		//printf( "%02X\n", val );
		str[6] = val & 0xFF;

		for ( i = 0; i < 8; ++i ) {
			printf( "%02X", str[i] );
		}
		printf( "\n" );

		scanf( "%s", recvtmp );
		len = strlen( (char*)recvtmp ) / 2;

		for( i = 0; i < len; ++i ) {
			recv[i] = (unsigned char) ( chartoint( recvtmp[i * 2] ) * 16 + 
					chartoint( recvtmp[i * 2 + 1] ) );
		}
		for ( i = 0; i < len; ++i ) {
			printf( "%d ", recv[i] );
		}

		val = modbus_crc( recv, len - 2 );
		printf( "\n%02X\n", val );
		if ( ( (val & 0xFF) != recv[len - 1] ) || ( ( (val >> 8) & 0xFF ) 
				!= recv[len - 2] ) ) {
			printf( "CRC ERROR\n" );
			continue;
		}

		k = 2;
		num = recv[k] / 4;
		printf( "num: %d\n", num );
		++k;

		for ( i = 0; i < num; ++i ) {
			fstr[0] = recv[k + 3];
			fstr[1] = recv[k + 2];
			fstr[2] = recv[k + 1];
			fstr[3] = recv[k];

			pf = (float*)(fstr);
			printf( "%.lf", *pf );
			if ( num - 1 != i ) {
				printf( ", " );
			}

			k += 4;
		}

		if ( num != 0 ) {
			printf( "\n" );
		}
	}
	
	return 0;
} 
