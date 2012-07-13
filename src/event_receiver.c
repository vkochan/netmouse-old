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
 
#include <stdio.h>
 
#include "event_pipe.h"
#include "log.h"
#include "config.h"
#include "mouse.h"
#include "keybd.h"
 
struct event_pipe *recv_pipe;

int n_x = 0;
int n_y = 0;

/*
 * this funnction is called from server listener and it
 * does nothing than just delegates to real "do" functions for mouse or keyboard
 */	
void on_recv_event_handler(struct input_event *evt)
{
	if (evt)
	{
		if (evt->type == INPUT_MOUSE_EVENT)
		{
			if (evt->action == EVENT_MOUSE_MOVE)
			{
				do_move_mouse(evt->point.x * n_x, evt->point.y * n_y);
			}
			else //its mouse click
			{
				do_mouse_click(evt->action);
			}
		}
		else if (evt->type == INPUT_KEYBD_EVENT)
		{
			do_keybd_press(evt->action, evt->keybd.vkey, evt->keybd.flags);
		}
	}
	else
	{
		LOG_ERROR("Cant process empty event\n");
	}
} 

void init_event_receiver()
{
	//TODO: move this to mouse module
	n_x = 65535.0f / GetSystemMetrics(SM_CXSCREEN) - 1;
	n_y = 65535.0f / GetSystemMetrics(SM_CYSCREEN) - 1;
	
	recv_pipe = create_event_pipe(NULL, get_server_port(), EVENT_PIPE_RECV);
	
	if (recv_pipe == NULL)
	{
		LOG_FATAL("Cant create recv_pipe\n");
	}
	
	recv_pipe->on_recv_event = on_recv_event_handler;
	
	if (open_event_pipe(recv_pipe) == 0)
	{
		LOG_DEBUG("recv_pipe is opened\n");
	}
	else
	{
		LOG_DEBUG("Cant open recv_pipe\n");
	}
}
 