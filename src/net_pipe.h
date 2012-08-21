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
 
#ifndef _EVENT_PIPE_HEADER_
#define _EVENT_PIPE_HEADER_

#include "input_event.h"

#include <winsock2.h>
#include <ws2tcpip.h>

#define SEND_SUCCESS 0
#define SEND_FAILURE 1
#define RECV_SUCCESS 2
#define RECV_FAILURE 3

#define NET_PIPE_CREATED           1
#define NET_PIPE_CONNECTED         2
#define NET_PIPE_DISCONNECTED      3
#define NET_PIPE_CONNECTING        4
#define NET_PIPE_CLOSED            5
#define NET_PIPE_LISTENNING        6
#define NET_PIPE_ACCEPTED          7

#define NET_PIPE_SERVER 1
#define NET_PIPE_CLIENT 2

#define OPT_PIPE_ASYNC    0
#define OPT_PIPE_STREAM   1
#define OPT_PIPE_PACKET   2

#define PIPE_PROT_TCP 0
#define PIPE_PROT_UDP 1

struct net_pipe
{
	SOCKET sck;
	SOCKET from_sck;
	int conn_state;
	struct addrinfo *addr_info;
	int op_status;    //last operation status
	int error;    //windows error number
	int type;    //server or client
	int opt;    //options: async, stream, packet
	int prot;    //protocol: tcp (default), udp
	int pkt_size;
	void *owner;    //object which owns net pipe
	void (* on_recv_data ) ( void *data, int len );
	void (* on_conn_state ) ( struct net_pipe *pipe );
	void (* on_error ) ( int errno );
};

int send_data( struct net_pipe *pipe, void *data, int len );

struct net_pipe *create_net_pipe( char *addr, char *port, int type );

int open_net_pipe( struct net_pipe *pipe );

void reopen_pipe ( struct net_pipe *pipe );

#endif
