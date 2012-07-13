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

#define EVENT_PIPE_CREATED           1
#define EVENT_PIPE_CONNECTED         2
#define EVENT_PIPE_DISCONNECTED      3
#define EVENT_PIPE_CONNECTING        4
#define EVENT_PIPE_CLOSED            5
#define EVENT_PIPE_LISTENNING        6
#define EVENT_PIPE_ACCEPTED          7

#define EVENT_PIPE_SEND 1
#define EVENT_PIPE_RECV 2

struct event_pipe
{
	SOCKET sck;
	SOCKET from_sck;
	int conn_state;
	struct addrinfo *addr_info;
	int op_status;
	int error;
	int type;
	void (* on_recv_event)(struct input_event *evt);
};

int do_send_event_pipe(struct event_pipe *evt_pipe, struct input_event *evt);

struct event_pipe * create_event_pipe(char *addr, char *port, int type);

int open_event_pipe(struct event_pipe *evt_pipe);

#endif