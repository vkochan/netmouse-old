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
 
#ifndef _NETCLICK_EVENT_SENDER_
#define _NETCLICK_EVENT_SENDER_

#include <stdio.h>

#include "defs.h"
#include "event_pipe.h"
#include "log.h"
#include "config.h"

struct event_pipe *send_pipe;

void init_event_sender()
{	
	if (get_server_addr() == NULL)
	{
		return;
	}
	
	send_pipe = create_event_pipe(get_server_addr(), get_server_port(), EVENT_PIPE_SEND);
	
	if (send_pipe == NULL)
	{
		LOG_FATAL("Cant create send_pipe\n");
	}
	else
	{
		LOG_DEBUG("send_pipe is initialized\n");
	}
	
	if (open_event_pipe(send_pipe) == 0)
	{
		LOG_DEBUG("send_pipe is opened\n");
	}
}

int send_event(struct input_event *evt)
{
	if (get_server_addr() == NULL)
	{
		return 0;
	}
	
	return do_send_event_pipe(send_pipe, evt);
}

#endif