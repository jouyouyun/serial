<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML><HEAD>
<META content="text/html; charset=windows-1252" http-equiv=Content-Type></HEAD>
<BODY><XMP>/* 		modbus_tcp.c 

   By P.Costigan email: phil@pcscada.com.au http://pcscada.com.au

   These library of functions are designed to enable a program send and
   receive data from a device that communicates using the Modbus tcp protocol.

   Copyright (C) 2000 Philip Costigan  P.C. SCADA LINK PTY. LTD.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.





   The functions included here have been derived from the 
   Modicon Modbus Protocol and Modbus TCP Protocol Reference Guide
   which can be obtained from Schneider at www.schneiderautomation.com.

   This code has its origins with 
   paul@pmcrae.freeserve.co.uk (http://www.pmcrae.freeserve.co.uk)
   who wrote a small program to read 100 registers from a modbus slave.
   
   I have used his code as a catalist to produce this more functional set
   of functions. Thanks paul.




*/

#include <fcntl.h>	/* File control definitions */
#include <stdio.h>	/* Standard input/output */
#include <string.h>
#include <termio.h>	/* POSIX terminal control definitions */
#include <sys/time.h>	/* Time structures for select() */
#include <unistd.h>	/* POSIX Symbolic Constants */
#include <errno.h>	/* Error definitions */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "modbus_tcp.h"

/*#define DEBUG*/	/* uncomment to see the data sent and received */









/***********************************************************************

   send_query( file_descriptor, query_string, query_length )

Function to send a query out to a modbus slave.
************************************************************************/

int send_query( int sfd, unsigned char *query, size_t string_length )
{
	int write_stat;

#ifdef DEBUG
	int i;
#endif


#ifdef DEBUG
/* Print to stderr the hex value of each character that is about to be */
/* sent to the modbus slave.					       */

	for( i = 0; i < string_length; i++ )
	{
		fprintf( stderr, "[%0.2X]", query[ i ] );
	}
	fprintf( stderr, "\n" );
#endif

	tcflush( sfd, TCIOFLUSH );	/* flush the input & output streams */
	
	write_stat = send( sfd, query, string_length, 0);

	tcflush( sfd, TCIFLUSH );	/* maybe not neccesary */

	return( write_stat );
}













/*********************************************************************

	modbus_response( response_data_array, query_array )

   Function to the correct response is returned and check for correctness.

   Returns:	string_length if OK
		0 if failed
		Less than 0 for exception errors

	Note: All functions used for sending or receiving data via
	      modbus return these return values.

**********************************************************************/

int modbus_response( unsigned char *data, unsigned char *query, int fd )
{
	int response_length;


	/* local declaration */
	int receive_response( unsigned char *received_string, int sfd );



	response_length = receive_response( data, fd );
	if( response_length > 0 )
	{
		
		/********** check for exception response *****/

		if( response_length && data[ 7 ] != query [ 7 ] )
		{
			/* return the exception value as a -ve number */
			response_length = 0 - data[ 8 ];
		}
	}

	return( response_length );
}








/***********************************************************************

	receive_response( array_for_data )

   Function to monitor for the reply from the modbus slave.
   This function blocks for timeout seconds if there is no reply.

   Returns:	Total number of characters received.
***********************************************************************/

int receive_response(  unsigned char *received_string, int sfd )
{

	int rxchar = -1;
	int data_avail = FALSE;
	int bytes_received = 0;
	int read_stat;

	int timeout = 1;			/* 1 second */
	int char_interval_timeout = 10000; 	/* 10 milliseconds. */

	int max_fds = 32;
	fd_set rfds;

	struct timeval tv;

	tv.tv_sec = timeout;
	tv.tv_usec = 0;

	FD_ZERO( &rfds );
	FD_SET( sfd, &rfds );


#ifdef DEBUG
	fprintf( stderr, "Waiting for response.\n");
#endif	

	/* wait for a response */
	data_avail = select( max_fds, &rfds, NULL, NULL, &tv );
	

	if( !data_avail )
	{
		bytes_received = 0; 
#ifdef DEBUG
		fprintf( stderr, "Comms time out\n" );
#endif
	}

	tv.tv_sec = 0;
	tv.tv_usec = char_interval_timeout;

	FD_ZERO( &rfds );
	FD_SET( sfd, &rfds );
	
	while( data_avail )
	{

		/* if no character at the buffer wait char_interval_timeout */
		/* before accepting end of response			    */

		if( select( max_fds, &rfds, NULL, NULL, &tv) > 0 )
		{
			read_stat = recv( sfd, &rxchar, 1, 0);

			if( read_stat < 0 )
			{
				bytes_received = SOCKET_FAILURE;
				data_avail = FALSE;
			}
			else
			{
				
				rxchar = rxchar & 0xFF;
				received_string[ bytes_received ++ ] = rxchar;
			}


			if( bytes_received >= MAX_RESPONSE_LENGTH )
			{
				bytes_received = SOCKET_FAILURE;
				data_avail = FALSE;
			}
#ifdef DEBUG
			/* display the hex code of each character received */
			fprintf( stderr, "<%0.2X>", rxchar );
#endif

		}
		else
		{
			data_avail = FALSE;
		}

	}	

#ifdef DEBUG
	fprintf( stderr, "\n" );
#endif

	if( bytes_received > 7 )
	{
		bytes_received -= 7;
	}


	return( bytes_received );
}







/***********************************************************************

	The following functions construct the required query into
	a modbus query packet.

***********************************************************************/

#define REQUEST_QUERY_SIZE 12	/* the following packets require          */
#define CHECKSUM_SIZE 2		/* 6 unsigned chars for the packet plus   */
				/* 2 for the checksum.                    */

void build_request_packet( int slave, int function, int start_addr,
			   int count, unsigned char *packet )
{
	int i;


	for( i = 0; i < 5 ; i++ ) packet[ i ] = 0;

	packet[ i++ ] = 6;
	packet[ i++ ] = slave;
	packet[ i++ ] = function;
	start_addr -= 1;
	packet[ i++ ] = start_addr >> 8;
	packet[ i++ ] = start_addr & 0x00ff;
	packet[ i++ ] = count >> 8;
	packet[ i ] = count &0x00ff;

}






/************************************************************************

	read_IO_status

	read_coil_stat_query and read_coil_stat_response interigate
	a modbus slave to get coil status. An array of coils shall be
	set to TRUE or FALSE according to the response from the slave.
	
*************************************************************************/

int read_IO_status( int function, int slave, int start_addr, int count, 
			    int *dest, int dest_size, int sfd )
{
	/* local declaration */
	int read_IO_stat_response( int *dest, int dest_size, int coil_count,
					unsigned char *query, int fd );

	int status;


	unsigned char packet[ REQUEST_QUERY_SIZE + CHECKSUM_SIZE ];
	build_request_packet( slave, function, start_addr, count, packet );

	if( send_query( sfd, packet, REQUEST_QUERY_SIZE ) > -1 )
	{
		status = read_IO_stat_response( dest, dest_size, 
						count, packet, sfd );
	}
	else
	{
		status = SOCKET_FAILURE;
	}



	return( status );
}



/************************************************************************

	read_coil_status_tcp

	reads the boolean status of coils and sets the array elements
	in the destination to TRUE or FALSE

*************************************************************************/

int read_coil_status_tcp( int slave, int start_addr, int count,
				int *dest, int dest_size, int sfd )
{
	int function = 0x01;
	int status;

	status = read_IO_status( function, slave, start_addr, count, 
						dest, dest_size, sfd );

	return( status );
}





/************************************************************************

	read_input_status_tcp

	same as read_coil_status but reads the slaves input table.

************************************************************************/

int read_input_status_tcp( int slave, int start_addr, int count,
				int *dest, int dest_size, int sfd )
{
	int function = 0x02;	/* Function: Read Input Status */
	int status;

	status = read_IO_status( function, slave, start_addr, count,
						 dest, dest_size, sfd );



	return( status );
}




/**************************************************************************

	read_IO_stat_response

	this function does the work of setting array elements to TRUE
	or FALSE.

**************************************************************************/

int read_IO_stat_response( int *dest, int dest_size, int coil_count,
				unsigned char *query, int fd )
{

	unsigned char data[ MAX_RESPONSE_LENGTH ];
	int raw_response_length;
	int temp, i, bit, dest_pos = 0;
	int coils_processed = 0;

	raw_response_length = modbus_response( data, query, fd );
	

	if( raw_response_length > 0 )
	{
		for( i = 0; i < ( data[8] ) && i < dest_size; i++ )
		{
			/* shift reg hi_byte to temp */
			temp = data[ 9 + i ] ;
			for( bit = 0x01; bit & 0xff && 
				coils_processed < coil_count; )
			{
				if( temp & bit )
				{
					dest[ dest_pos ] = TRUE;
				}
				else
				{
					dest[ dest_pos ] = FALSE;
				}
				coils_processed++;
				dest_pos++;
				bit = bit << 1;
			}
		}
	}

	return( raw_response_length );
}





/************************************************************************

	read_registers

	read the data from a modbus slave and put that data into an array.

************************************************************************/

int read_registers( int function, int slave, int start_addr, int count, 
			  int *dest, int dest_size, int sfd )
{
	/* local declaration */
	int read_reg_response( int *dest, int dest_size, 
					unsigned char *query, int fd );

	int status;


	unsigned char packet[ REQUEST_QUERY_SIZE + CHECKSUM_SIZE ];
	build_request_packet( slave, function, start_addr, count, packet );

	if( send_query( sfd, packet, REQUEST_QUERY_SIZE ) > -1 )
	{
		status = read_reg_response( dest, dest_size, packet, sfd );
	}
	else
	{
		status = SOCKET_FAILURE;
	}
	
	return( status );

}



/************************************************************************

	read_holding_registers

	Read the holding registers in a slave and put the data into
	an array.

*************************************************************************/

int read_holding_registers_tcp( int slave, int start_addr, int count,
				int *dest, int dest_size, int sfd )
{
	int function = 0x03;    /* Function: Read Holding Registers */
	int status;

	if( count > MAX_READ_REGS )
	{
		count = MAX_READ_REGS; 
#ifdef DEBUG
		fprintf( stderr, "Too many registers requested.\n" );
#endif
	}

	status = read_registers( function, slave, start_addr, count,
						dest, dest_size, sfd );

	return( status);
}





/************************************************************************

	read_input_registers

	Read the inputg registers in a slave and put the data into
	an array. 

*************************************************************************/

int read_input_registers_tcp( int slave, int start_addr, int count,
				int *dest, int dest_size, int sfd )
{
	int function = 0x04;	/* Function: Read Input Reqisters */
	int status;

	if( count > MAX_INPUT_REGS )
	{
		count =  MAX_INPUT_REGS; 
#ifdef DEBUG
		fprintf( stderr, "Too many input registers requested.\n" );
#endif
	}

	status = read_registers( function, slave, start_addr, count,
						dest, dest_size, sfd );

	return( status );
}





/************************************************************************

	read_reg_response

	reads the response data from a slave and puts the data into an
	array.

************************************************************************/

int read_reg_response( int *dest, int dest_size, unsigned char *query, int fd )
{

	unsigned char data[ MAX_RESPONSE_LENGTH ];
	int raw_response_length;
	int temp,i;

	

	raw_response_length = modbus_response( data, query, fd );
	if( raw_response_length > 0 )
		raw_response_length -= 2;
	
	
	if( raw_response_length > 0 )
	{
		for( i = 0; 
			i < ( data[8] * 2 ) && i < (raw_response_length / 2); 
									i++ )
		{
			/* shift reg hi_byte to temp */
			temp = data[ 9 + i *2 ] << 8;    
			/* OR with lo_byte           */
			temp = temp | data[ 10 + i * 2 ];
		
			dest[i] = temp; 
		}
	}
	return( raw_response_length );
}










/***********************************************************************

	preset_response

	Gets the raw data from the input stream.

***********************************************************************/

int preset_response( unsigned char *query, int fd ) 
{
	unsigned char data[ MAX_RESPONSE_LENGTH ];
	int raw_response_length;
	
	raw_response_length = modbus_response( data, query, fd );

	return( raw_response_length );
}






/*************************************************************************

	set_single

	sends a value to a register in a slave. 

**************************************************************************/

int set_single( int function, int slave, int addr, int value, int fd )
{

	int status;
	int i;

	unsigned char packet[ REQUEST_QUERY_SIZE ];


	for( i = 0; i < 5; i++ ) packet[ i ] = 0;
	packet[ i++ ] = 6;	
	packet[ i++ ] = slave;
	packet[ i++ ] = function;
	addr -= 1;
	packet[ i++ ] = addr >> 8;
	packet[ i++ ] = addr & 0x00FF;
	packet[ i++ ] = value >> 8;
	packet[ i++ ] = value & 0x00FF;

	if( send_query( fd, packet, i ) > -1 )
	{
		status = preset_response( packet, fd );
	}
	else
	{
		status = SOCKET_FAILURE;
	}

	return( status );
}






/*************************************************************************

	force_single_coil

	turn on or off a single coil on the slave device

*************************************************************************/

int force_single_coil_tcp( int slave, int coil_addr, int state, int fd )
{
	int function = 0x05;
	int status;

	if( state ) state = 0xFF00;

	status = set_single( function, slave, coil_addr, state, fd );

	return( status );
}





/*************************************************************************

	preset_single_register

	sets a value in one holding register in the slave device

*************************************************************************/

int preset_single_register_tcp(int slave, int reg_addr, int value, int fd )
{
	int function = 0x06;
	int status;

	status = set_single( function, slave, reg_addr, value, fd );

	return( status );
}





/************************************************************************

	set_multiple_coils

	Takes an array of ints and sets or resets the coils on a slave
	appropriatly.

*************************************************************************/

#define PRESET_QUERY_SIZE 210

int set_multiple_coils_tcp( int slave, int start_addr, int coil_count,
				int *data, int fd )
{
	int byte_count;
	int i, bit, packet_size, data_legth;
	int coil_check = 0;
	int data_array_pos = 0;
	int status;

	unsigned char packet[ PRESET_QUERY_SIZE ];

	if( coil_count > MAX_WRITE_COILS )
	{
		coil_count = MAX_WRITE_COILS;
#ifdef DEBUG
		fprintf( stderr, "Writing to too many coils.\n" );
#endif
	}
	for( packet_size = 0; packet_size < 5; packet_size++ )
		packet[ packet_size ] = 0;
	packet[ packet_size++ ] = 6;
	packet[ packet_size++ ] = slave;
	packet[ packet_size++ ] = 0x0F;
	start_addr -= 1;
	packet[ packet_size++ ] = start_addr >> 8;
	packet[ packet_size++ ] = start_addr & 0x00FF;
	packet[ packet_size++ ] = coil_count >> 8;
	packet[ packet_size++ ] = coil_count & 0x00FF;
	byte_count = (coil_count / 8) + 1;
	packet[ packet_size ] = byte_count;

	bit = 0x01;

	for( i = 0; i < byte_count; i++)
	{
		packet[ ++packet_size ] = 0;

		while( bit & 0xFF && coil_check++ < coil_count )
		{
			if( data[ data_array_pos++ ] )
			{
				packet[ packet_size ] |= bit;
			}
			else
			{
				packet[ packet_size ] &=~ bit;
			}
			bit = bit << 1;
		}
		bit = 0x01;
	}

	data_legth = packet_size - 5;
	packet[ 4 ] = data_legth >> 8;
	packet[ 5 ] = data_legth & 0x00FF;


	if( send_query( fd, packet, ++packet_size ) > -1 )
	{
		status = preset_response( packet, fd );
	}
	else
	{
		status = SOCKET_FAILURE;
	}

	return( status );
}





/*************************************************************************

	preset_multiple_registers

	copy the values in an array to an array on the slave.

***************************************************************************/

int preset_multiple_registers_tcp( int slave, int start_addr, 
				    int reg_count, int *data, int fd )
{
	int byte_count, i, packet_size, data_legth;
	int status;

	unsigned char packet[ PRESET_QUERY_SIZE ];

	if( reg_count > MAX_WRITE_REGS )
	{
		reg_count = MAX_WRITE_REGS;
#ifdef DEBUG
		fprintf( stderr, "Trying to write to too many registers.\n" );
#endif
	}

	for( packet_size = 0; packet_size < 5; packet_size++ )
		packet[ packet_size ] = 0;
	packet[ packet_size++ ] = 6;
	packet[ packet_size++ ] = slave;
	packet[ packet_size++ ] = 0x10;
	start_addr -= 1;
	packet[ packet_size++ ] = start_addr >> 8;
	packet[ packet_size++ ] = start_addr & 0x00FF;
	packet[ packet_size++ ] = reg_count >> 8;
	packet[ packet_size++ ] = reg_count & 0x00FF;
	byte_count = reg_count * 2;
	packet[ packet_size ] = byte_count;

	for( i = 0; i < reg_count; i++ )
	{
		packet[ ++packet_size ] = data[ i ] >> 8;
		packet[ ++packet_size ] = data[ i ] & 0x00FF;
	}

	data_legth = packet_size - 5;
	packet[ 4 ] = data_legth >> 8;
	packet[ 5 ] = data_legth & 0x00FF;

	if( send_query( fd, packet, ++packet_size ) > -1 )
	{
		status = preset_response( packet, fd );
	}
	else
	{
		status = SOCKET_FAILURE;
	}

	return( status );
}








#define MODBUS_TCP_PORT 502


int set_up_tcp( char *ip_address )
{
	int sfd;
	struct sockaddr_in server;
	int connect_stat;

	sfd = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );

	server.sin_family = AF_INET;
	server.sin_port = htons( MODBUS_TCP_PORT );
	server.sin_addr.s_addr = inet_addr(ip_address);

	if( sfd >= 0 )
	{
		connect_stat = connect( sfd, (struct sockaddr *)&server,
						sizeof(struct sockaddr_in) );
	
		if( connect_stat < 0 )
		{
			//plc_log_errmsg( 0, "\nConnect - error %d\n", 
			//				connect_stat );
			close( sfd );
			sfd = -1;
			// exit( connect_stat );
		}
	}

	return( sfd );
}






/****************************************************************************
*****************************************************************************
*****************************************************************************

    THIS SECTION IS DEDICATED TO MODBUS TCP SLAVE FUNCTIONS.

*****************************************************************************
*****************************************************************************
****************************************************************************/









int get_slave_query( int *slave_addr, int *query, int *start_addr, 
			int *point_count, int *data, int sfd )
{

	unsigned char raw_data[ MAX_RESPONSE_LENGTH ];
	int response_length;
	int i,temp;	


	response_length = receive_response( raw_data, sfd );

	
        if( response_length > 0 )
        {

		*slave_addr = raw_data[ 6 ];
                *query = raw_data[ 7 ]; 
		*start_addr = raw_data[ 8 ] << 8;
		*start_addr = *start_addr | raw_data[ 9 ];
		*point_count = raw_data[ 10 ] << 8;
		*point_count = *point_count | raw_data[ 11 ];


                for( i = 0;
                        i < ( data[12] * 2 ) && i < (response_length / 2);
                                                                        i++ )
                {
                        /* shift reg hi_byte to temp */
                        temp = data[ 13 + i *2 ] << 8;
                        /* OR with lo_byte           */
                        temp = temp | data[ 14 + i * 2 ];

                        data[i] = temp;
                }

        }
 
	return( response_length );	

}










int set_up_tcp_slave( void )
{

        int sfd;
        struct sockaddr_in client;
	int bind_stat;
        int accept_stat;
        int listen_stat;




        client.sin_family = AF_INET;
        client.sin_port = htons( MODBUS_TCP_PORT );
        client.sin_addr.s_addr = INADDR_ANY;

        sfd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

        if( sfd >= 0 )
        {

        	bind_stat = bind( sfd, &client, sizeof( client ) );
		if( bind_stat < 0 )
        	{
                	//plc_log_errmsg( 0,"bind failed %s\n", 
			//			strerror( errno) );
                	close( sfd );
			sfd = -1;
                	//exit( bind_stat );
        	}
	}

	if( sfd >= 0 )
	{
        	listen_stat = listen( sfd, 1 );
        	if( listen_stat != 0 )
        	{
                	//plc_log_errmsg( "listen FAILED with %s\n", 
			//			strerror( errno ) );
                	close( sfd );
			sfd = -2;
			if( listen_stat > 0 ) sfd = -3;
	               	//exit(listen_stat);
		}
        

        	if( listen_stat == 0 )
        	{
                	accept_stat = accept( sfd, NULL, NULL );
        	
        	

		        if( accept_stat < 0 )
        		{
                        	//plc_log_errmsg( 0, "\nConnect - error %d\n",
                        	//                              connect_stat );
                
                		close( sfd );
                		sfd = -4;
                		//exit( accept_stat );
        		}
        	}
	}
        return( sfd );
}
 
</XMP></BODY></HTML>
