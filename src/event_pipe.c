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

#include "process.h"
#include "event_pipe.h"
#include "log.h"
#include "input_handler.h"

#define CONNECTION_CHECK_TIME 5000

WSADATA wsaData;

DECLARE_PROCESS(event_receiver);
DECLARE_PROCESS(server_listener);
DECLARE_PROCESS(client_connector);

struct input_event * alloc_input_event()
{
	struct input_event *new_evt = (struct input_event *)malloc(sizeof(struct input_event));
	
	if ( new_evt )
	{
		memset( new_evt, sizeof(struct input_event), 0 );
		return new_evt;
	}
	
	return NULL;
}

void free_input_event( struct input_event *evt )
{
	free(evt);
}

void set_event_pipe_state( struct event_pipe *evt_pipe, int state )
{
	evt_pipe->conn_state = state;
	
	if ( evt_pipe->on_conn_state )
	{
	    evt_pipe->on_conn_state( evt_pipe );
	}
}

int get_event_pipe_state( struct event_pipe *evt_pipe )
{
	return evt_pipe->conn_state;
}

/*
 * reads events from remote side socket
 */
DWORD WINAPI event_receiver( void *data )
{
	struct input_event *evt = NULL;
	struct event_pipe *evt_pipe = (struct event_pipe *)data;
	
	int result;
	
	if ( !evt_pipe )
	{
		return 1;
	}
	
	if ( !evt_pipe->on_recv_event )
	{
		return 1;
	}
	
	evt = alloc_input_event();
	
	if ( evt == NULL )
	{
		return 1;
	};

	// Receive until the peer shuts down the connection
	do {
		memset( &evt, sizeof(struct input_event), 0 );
		
		//LOG_DEBUG("try to receive event ...\n");
		result = recv( evt_pipe->from_sck, (char *)evt, sizeof(struct input_event), 0 );
		
		if (result > 0) {
			evt_pipe->on_recv_event( evt );
		} else
		{
			if (result == 0)
			{
				evt_pipe->from_sck = INVALID_SOCKET;
				return 1;
			}
			else 
			{
				LOG_ERROR( "recv failed: %d", WSAGetLastError() );
				closesocket( evt_pipe->from_sck );
				evt_pipe->from_sck = INVALID_SOCKET;
				return 1;
			}
		}		

	} while (1);
}

/*
 * listens for events from client
 */
DWORD WINAPI server_listener( void *data )
{
	struct event_pipe *evt_pipe = (struct event_pipe *)data;
	SOCKET client = INVALID_SOCKET;
		
	if (evt_pipe == NULL)
	{
		return 1;
	}
	
	set_event_pipe_state ( evt_pipe, EVENT_PIPE_LISTENNING );
	
	while(1)
	{
		// Accept a client socket
		client = accept(evt_pipe->sck, NULL, NULL);
		if (client == INVALID_SOCKET) {
			//LOG_ERROR("Error while accepting client: %ld\n", WSAGetLastError());
			closesocket(client);
		}
		else
		{	
			set_event_pipe_state( evt_pipe, EVENT_PIPE_ACCEPTED );
			
			evt_pipe->from_sck = client;
			
			if (evt_pipe->on_recv_event != NULL)
			{
				RUN_PROCESS(event_receiver, evt_pipe);
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
	struct event_pipe *evt_pipe = (struct event_pipe *)data;
	int result;
	
	if ( get_event_pipe_state( evt_pipe ) == EVENT_PIPE_CONNECTING )
	{
	    return 0;
	}
	
	set_event_pipe_state( evt_pipe, EVENT_PIPE_CONNECTING );
	
	while(1)
	{
		// Connect to server.
		result = connect( evt_pipe->sck, evt_pipe->addr_info->ai_addr, (int)evt_pipe->addr_info->ai_addrlen );
		
		if (result == SOCKET_ERROR)
		{
		    int err = WSAGetLastError();
			
			LOG_ERROR("Error while connection to server: %ld", err );
		    
			if ( err == WSAEISCONN )
			{
			    reopen_event_pipe( evt_pipe );
				continue;
			}
			
			Sleep( CONNECTION_CHECK_TIME );
		}
		else
		{
			set_event_pipe_state( evt_pipe, EVENT_PIPE_CONNECTED );
			
			u_long iMode = 1;
			ioctlsocket( evt_pipe->sck, FIONBIO, &iMode );
			
			int one = 1;
			setsockopt( evt_pipe->sck, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one) );
			return 0;
		}
	}
}

/*
 * creates event pipe object and related socket
 */
struct event_pipe *create_event_pipe( char *addr, char *port, int type )
{
	struct event_pipe *evt_pipe = NULL;
	struct addrinfo *addr_info = NULL, hints;
	SOCKET sck = INVALID_SOCKET;
	int iResult;

	evt_pipe = (struct event_pipe *)malloc( sizeof( struct event_pipe ) );
	memset( evt_pipe, 0, sizeof(evt_pipe) );
	
	if ( evt_pipe == NULL )
	{
		LOG_FATAL("Cant alloc memory for event_pipe");
	}
	
	ZeroMemory( &hints, sizeof(hints) );
	
	if ( type == EVENT_PIPE_RECV )
	{
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;
	}
	else if( type == EVENT_PIPE_SEND )
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
	
	if (type == EVENT_PIPE_RECV)
	{
		iResult = bind(sck, addr_info->ai_addr, (int)addr_info->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			LOG_FATAL("bind failed with error: %d", WSAGetLastError());
			closesocket(sck);
			WSACleanup();
			return NULL;
		}
	}
	
	evt_pipe->addr_info = addr_info;
	evt_pipe->sck = sck;
	evt_pipe->conn_state = EVENT_PIPE_CREATED;
	evt_pipe->type = type;
	
	return evt_pipe;
}

/*
 * does socket specific logic: connects to server if its
 * sending pipe or puts server into listening state if its
 * receiving pipe
 */
int open_event_pipe(struct event_pipe *evt_pipe)
{
	if(evt_pipe)
	{
		if (evt_pipe->type == EVENT_PIPE_SEND)
		{
			RUN_PROCESS(client_connector, evt_pipe);
			return 0;
		}
		else if (evt_pipe->type == EVENT_PIPE_RECV)
		{	
			if (listen(evt_pipe->sck, SOMAXCONN) == SOCKET_ERROR) 
			{
				LOG_FATAL("Listen failed with error: %ld", WSAGetLastError());
				closesocket(evt_pipe->sck);
				WSACleanup();
				return 1;
			}
			else
			{
				RUN_PROCESS(server_listener, evt_pipe);
				return 0;
			}
		}
		
	}
	
	return 1;
}

/*
 * general function for sending input event through socket
 */
int do_send_event_pipe(struct event_pipe *evt_pipe, struct input_event *evt)
{
	if( evt_pipe->conn_state == EVENT_PIPE_CONNECTED )
	{
		int iResult;
		int errno;
		
		// Send an initial buffer
		iResult = send(evt_pipe->sck, (char *)evt, sizeof(struct input_event), 0);
		
		if (iResult == SOCKET_ERROR) {
			disable_remote_events(); //unblock input messages for other apps
			
			errno = WSAGetLastError();
			
			LOG_ERROR("send failed: %d", errno);
			
			evt_pipe->op_status = SEND_FAILURE;
			evt_pipe->error = WSAGetLastError();
			
			set_event_pipe_state( evt_pipe, EVENT_PIPE_DISCONNECTED );
			
			RUN_PROCESS( client_connector, evt_pipe );
			return -1;
		}
		else
		{
			evt_pipe->op_status = SEND_SUCCESS;
		}
		
		return 0;
	}
	
	return -1;
}

void reopen_event_pipe ( struct event_pipe *evt_pipe )
{
	closesocket( evt_pipe->sck );
	
	// Create a SOCKET for connecting to server
	evt_pipe->sck = socket( evt_pipe->addr_info->ai_family, evt_pipe->addr_info->ai_socktype, evt_pipe->addr_info->ai_protocol );
}

/*
 * initialization of event pipe module
 */
void init_event_pipe()
{
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	
	if ( iResult != 0 ) 
	{
		LOG_FATAL("WSAStartup failed: %d", iResult);
	}
	
	LOG_DEBUG("net module is initialized", NULL);
}

/*
 * unloads socket specific library initialization
 */
void cleanup_event_pipe()
{
	WSACleanup();
}