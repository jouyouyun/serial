#include "my_serial.h"
#include <limits.h>
#include <assert.h>

//char buf_int[512] = "0";

long int
str_to_int( char *str )
{
	long ll;
	assert( str );

	errno = 0;
	ll = strtol(str, NULL, 16);
	if ( (errno == ERANGE && (ll == LONG_MAX || ll == LONG_MIN )) 
			|| ( errno != 0 && ll == 0 ) ) {
		perror( "strtol" );
		exit(EXIT_FAILURE);
	}

	printf( "%ld\n", ll );

	return ll;
}

void
str_delim( const char *str, const char *delim )
{
	char *p;
	char buf[16];
	char *tmp = NULL;

	assert( str );
	assert( delim );

	tmp = (char*)malloc( sizeof(char) * (strlen(str) + 1) ) ;
	memset( tmp, 0, strlen(tmp) + 1 );
	memcpy( tmp, str, strlen(str) + 1 );

	p = strtok( tmp, delim );
	sprintf(buf, "%ld%s", str_to_int( p ), ", " );
	memcpy( buf_int, buf, strlen(buf) );
	while ( ( p = strtok( NULL, delim )) != NULL ) {
		sprintf(buf, "%ld%s", str_to_int( p ), ", " );
		strcat( buf_int, buf );
		//str_to_int( p );
	}
	free(tmp);
	printf( "str: %s\n", str );
}

/*
int
main( int argc, char *argv[] )
{
	char str[] = "0x11, 0x34, 0xff, 0x24";

	str_delim( str, "," );
	printf( "buf_int: %s\n", buf_int );

	return 0;
}
*/
