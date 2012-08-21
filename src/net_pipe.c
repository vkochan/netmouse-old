/* 
 * Copyright (C) 2012 Vadim Kochan
 *  
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA.
 * 
 * Author:   Vadim Kochan <vadim4j@gmail.com>
 */

#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "process.h"
#include "net_pipe.h"
#include "log.h"
#include "input_handler.h"

#define CONNECTION_CHECK_TIME 5000

struct data_hdr
{
    int len;
};

WSADATA wsaData;

DECLARE_PROCESS(pipe_receiver);
DECLARE_PROCESS(server_listener);
DECLARE_PROCESS(client_connector);

void close_socket( SOCKET *sck )
{
    closesocket( *sck );
	*sck = INVALID_SOCKET;
}

void set_pipe_state( struct net_pipe *pipe, int state )
{
	pipe->conn_state = state;
	
	if ( pipe->on_conn_state )
	{
	    pipe->on_conn_state( pipe );
	}
}

int get_pipe_state( struct net_pipe *pipe )
{
	return pipe->conn_state;
}

void *recv_stream( int *len )
{
    
}

int recv_packet( void *pkt_buff, int len )
{

}

int recv_hdr( SOCKET sck, struct data_hdr *hdr )
{
    return recv( sck, (char *)hdr, sizeof( struct data_hdr ), 0 );
}

/*
 * reads data from remote socket
 */
DWORD WINAPI pipe_receiver( void *data )
{
	struct net_pipe *pipe = (struct net_pipe *)data;
	
	if ( !pipe )
	{
		return 1;
	}
	
	//how we can notify about received data w/o handler?
	if ( !pipe->on_recv_data )
	{
		return 1;
	}
	

	struct data_hdr hdr = { 0 };
    void *pkt_buff = malloc( pipe->pkt_size );
    memset( pkt_size, 0 , pipe->pkt_size );

	// Receive until the peer shuts down the connection
	do {
		
		//first of all lets get our data_hdr which tell us
		//how much data we should receive from pipe 
		
		if ( recv_data_hdr( pipe->from_sck, &hdr ) <= 0 )
		{
			goto recv_err;
		}
		
		assert( hdr.len > 0 );

		void *recvd_buffer = malloc( hdr.len );

		assert( recvd_buffer );

		int recvd_bytes = 0;

		while( recvd_bytes < hdr.len )
		{
            int result = recv( pipe->from_sck, (char *)( recvd_buffer + recvd_bytes ), hdr.len - recvd_bytes, 0 );

            if ( result > 0 ) 
            {
			    recvd_bytes += result;
			} 
			else
			{
				goto recv_err;
			}
		}

		pipe->on_recv_data( recvd_buffer, recvd_bytes );		

	} while (1);

recv_err:
	LOG_ERROR( "recv failed: %d", WSAGetLastError() );
	close_socket( &(pipe->from_sck) );
	return 1;    
}

/*
 * listens for events from client
 */
DWORD WINAPI server_listener( void *data )
{
	struct net_pipe *pipe = (struct net_pipe *)data;
	SOCKET client = INVALID_SOCKET;
		
	if ( pipe == NULL )
	{
		return 1;
	}
	
	set_pipe_state ( pipe, NET_PIPE_LISTENNING );
	
	while( 1 )
	{
		// Accept a client socket
		client = accept(pipe->sck, NULL, NULL);
		if (client == INVALID_SOCKET) {
			//LOG_ERROR("Error while accepting client: %ld\n", WSAGetLastError());
			closesocket(client);
		}
		else
		{	
			set_pipe_state( pipe, NET_PIPE_ACCEPTED );
			
			pipe->from_sck = client;
			
			if (pipe->on_recv_data != NULL)
			{
				RUN_PROCESS( pipe_receiver, pipe );
			}
		}
	}
}

/*
 * tries to connect to server when connection is closed
 * runs in separated thread
 */ 
DWORD WINAPI client_connector( void *data )
{
	struct net_pipe *pipe = (struct net_pipe *)data;
	int result;
	
	if ( get_pipe_state( pipe ) == NET_PIPE_CONNECTING )
	{
	    return 0;
	}
	
	set_pipe_state( pipe, NET_PIPE_CONNECTING );
	
	while( 1 )
	{
		// Connect to server.
		result = connect( pipe->sck, pipe->addr_info->ai_addr, (int)pipe->addr_info->ai_addrlen );
		
		if (result == SOCKET_ERROR)
		{
		    int err = WSAGetLastError();
			
			LOG_ERROR("Error while connection to server: %ld", err );
		    
			if ( err == WSAEISCONN )
			{
			    //we should to re-create socket
			    //for re-connection to our server
			    reopen_pipe( pipe );
				continue;
			}
			
			Sleep( CONNECTION_CHECK_TIME );
		}
		else
		{
			set_pipe_state( pipe, NET_PIPE_CONNECTED );
			
			u_long iMode = 1;
			ioctlsocket( pipe->sck, FIONBIO, &iMode );
			
			int on = 1;
			setsockopt( pipe->sck, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on) );
			return 0;
		}
	}
}

/*
 * creates event pipe object and related socket
 */
struct net_pipe *create_net_pipe( char *addr, char *port, int type )
{
	struct net_pipe *pipe = NULL;
	struct addrinfo *addr_info = NULL, hints;
	SOCKET sck = INVALID_SOCKET;
	int iResult;

	pipe = (struct net_pipe *)malloc( sizeof( struct net_pipe ) );
	memset( pipe, 0, sizeof( struct net_pipe ) );
	
	if ( pipe == NULL )
	{
		LOG_FATAL("Cant alloc memory for net_pipe");
	}
	
	ZeroMemory( &hints, sizeof(hints) );
	
	if ( type == NET_PIPE_SERVER )
	{
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;
	}
	else if( type == NET_PIPE_CLIENT )
	{
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
	}

	// Resolve the server address and port
	iResult = getaddrinfo( addr, port, &hints, &addr_info );
	if (iResult != 0) {
		LOG_FATAL("getaddrinfo failed for %s:%s : error %d",addr, port, iResult);
		WSACleanup();
		return NULL;
	}

	// Create a SOCKET for connecting to server
	sck = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol);

	if (sck == INVALID_SOCKET) {
		LOG_FATAL("Error at socket(): %ld", WSAGetLastError());
		WSACleanup();
		return NULL;
	}
	
	if (type == NET_PIPE_SERVER)
	{
		iResult = bind(sck, addr_info->ai_addr, (int)addr_info->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			LOG_FATAL("bind failed with error: %d", WSAGetLastError());
			closesocket(sck);
			WSACleanup();
			return NULL;
		}
	}
	
	pipe->addr_info = addr_info;
	pipe->sck = sck;
	pipe->conn_state = NET_PIPE_CREATED;
	pipe->type = type;
	
	return pipe;
}

/*
 * does socket specific logic: connects to server if it is
 * sending pipe or puts server into listening state if it is
 * receiving pipe
 */
int open_net_pipe(struct net_pipe *pipe)
{
	if ( pipe )
	{
		if ( pipe->type == NET_PIPE_CLIENT )
		{
			RUN_PROCESS( client_connector, pipe );
			return 0;
		}
		else if ( pipe->type == NET_PIPE_SERVER )
		{	
			if ( listen( pipe->sck, SOMAXCONN ) == SOCKET_ERROR ) 
			{
				LOG_FATAL( "Listen failed with error: %ld", WSAGetLastError() );
				closesocket( pipe->sck );
				WSACleanup();
				return 1;
			}
			else
			{
				RUN_PROCESS(server_listener, pipe);
				return 0;
			}
		}
		
	}
	
	return 1;
}

void *wrap_data_hdr( void *data, int len )
{
    assert( data != NULL );
    assert( len > 0 );
    
    int hdr_len = sizeof( struct data_hdr );
    int wrap_len = hdr_len + len;

    void *wrapp_buf = malloc( wrap_len );

    assert( wrapp_buf != NULL );
    
    memset( wrapp_buf,  0, wrap_len );
    struct data_hdr *hdr = ( struct data_hdr * )wrapp_buf;
    hdr->len = len;

    void *buff = wrapp_buf + hdr_len;
    memcpy( buff, data, len );

    return wrapp_buf;
};

/*
 * general function for sending data through socket
 */
int send_data( struct net_pipe *pipe, void *data, int len )
{
	assert( pipe != NULL );

	if( pipe->conn_state == NET_PIPE_CONNECTED )
	{
		int result;
		int errno;
		
		void *pkt_data = wrap_data_hdr( data, len );

		// Send an initial buffer
		result = send( pipe->sck, (char *)pkt_data, sizeof( struct data_hdr ) + len, 0 );
		
		if ( result == SOCKET_ERROR ) 
		{
			errno = WSAGetLastError();
			
			LOG_ERROR("send failed: %d", errno);
			
			pipe->op_status = SEND_FAILURE;
			pipe->error = WSAGetLastError();
			
			set_pipe_state( pipe, NET_PIPE_DISCONNECTED );
			
			RUN_PROCESS( client_connector, pipe );
			return -1;
		}
		else
		{
			pipe->op_status = SEND_SUCCESS;
		}
		
		return 0;
	}
	
	return -1;
}

void reopen_pipe ( struct net_pipe *pipe )
{
	closesocket( pipe->sck );
	
	// Create a SOCKET for connecting to server
	pipe->sck = socket( pipe->addr_info->ai_family, pipe->addr_info->ai_socktype, pipe->addr_info->ai_protocol );
}

/*
 * initialization of event pipe module
 */
void init_net_pipe()
{
	int result;

	// Initialize Winsock
	result = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
	
	if ( result != 0 ) 
	{
		LOG_FATAL( "WSAStartup failed: %d", result );
	}
	
	LOG_DEBUG( "net_pipe is initialized", NULL );
}

/*
 * unloads socket specific library initialization
 */
void cleanup_net_pipe()
{
	WSACleanup();
}