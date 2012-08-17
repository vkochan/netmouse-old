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
 
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mouse.h"
#include "input_event.h"
#include "log.h"
#include "config.h"
#include "input_handler.h"
#include "screen.h"

/*
 * converts windows raw mouse event type into custom type
 */
int convert_to_mouse_net_event( int event_val )
{
	switch(event_val)
	{
		case RI_MOUSE_LEFT_BUTTON_DOWN:
			return EVENT_MOUSE_LEFT_DOWN;
		case RI_MOUSE_LEFT_BUTTON_UP:
			return EVENT_MOUSE_LEFT_UP;
		case RI_MOUSE_MIDDLE_BUTTON_DOWN:
			return EVENT_MOUSE_MIDDLE_DOWN;
		case RI_MOUSE_MIDDLE_BUTTON_UP:
			return EVENT_MOUSE_MIDDLE_UP;
		case RI_MOUSE_RIGHT_BUTTON_DOWN:
			return EVENT_MOUSE_RIGHT_DOWN;
		case RI_MOUSE_RIGHT_BUTTON_UP:
			return EVENT_MOUSE_RIGHT_UP;
		case RI_MOUSE_WHEEL:
			return EVENT_MOUSE_WHEEL;
	}

	return -1;
}

/*
 * converts custom mouse event type into windows event mouse value
 * for send input function
 */
int convert_net_to_mouse_event( int net_event )
{
	switch(net_event)
	{
		case EVENT_MOUSE_LEFT_DOWN:
			return MOUSEEVENTF_LEFTDOWN;
		case EVENT_MOUSE_LEFT_UP:
			return MOUSEEVENTF_LEFTUP;
		case EVENT_MOUSE_MIDDLE_DOWN:
			return MOUSEEVENTF_MIDDLEDOWN;
		case EVENT_MOUSE_MIDDLE_UP:
			return MOUSEEVENTF_MIDDLEUP;
		case EVENT_MOUSE_RIGHT_DOWN:
			return MOUSEEVENTF_RIGHTDOWN;
		case EVENT_MOUSE_RIGHT_UP:
			return MOUSEEVENTF_RIGHTUP;
		case EVENT_MOUSE_WHEEL:
			return MOUSEEVENTF_WHEEL;
		case EVENT_MOUSE_MOVE:
			return MOUSEEVENTF_MOVE;
	}

	return -1;
}

char *get_action_name ( int act )
{
	switch( act )
	{
		case EVENT_MOUSE_LEFT_DOWN:
			return "LEFT DOWN";
		case EVENT_MOUSE_LEFT_UP:
			return "LEFT UP";
		case EVENT_MOUSE_MIDDLE_DOWN:
			return "MIDDLE DOWN";
		case EVENT_MOUSE_MIDDLE_UP:
			return "MIDDLE UP";
		case EVENT_MOUSE_RIGHT_DOWN:
			return "RIGHT DOWN";
		case EVENT_MOUSE_RIGHT_UP:
			return "RIGHT UP";
		case EVENT_MOUSE_WHEEL:
			return "WHEEL";
		case EVENT_MOUSE_MOVE:
			return "MOVE";
	}

	return "UNKNOWN";
}

/*
 * emulates mouses move by absolute coordinates
 */
void do_mouse_move( int x, int y )
{
    struct screen_config scr_cfg; 
	
	get_screen_config(&scr_cfg);
	
	int n_x = 65535.0f / scr_cfg.max_x - 1;
	int n_y = 65535.0f / scr_cfg.max_y - 1;	
	
	POINT pt;
	GetCursorPos( &pt );
	
	INPUT in;
	
	in.type = INPUT_MOUSE;
	in.mi.time = 0;
	in.mi.dwExtraInfo = ( ULONG_PTR )NULL;
	in.mi.dx = x * n_x;
	in.mi.dy = y * n_y;
	in.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
	
	SendInput( 1, &in, sizeof(INPUT) );
}

/*
 * emulates mouse click event
 */
void do_mouse_click ( int btn_click )
{
	INPUT in;
	
	in.type = INPUT_MOUSE;
	in.mi.time = 0;
	in.mi.dwExtraInfo = ( ULONG_PTR )NULL;
	in.mi.dx = 0;
	in.mi.dy = 0;
	in.mi.dwFlags = convert_net_to_mouse_event( btn_click );
	
	SendInput( 1, &in, sizeof(INPUT) );
}

/*
 * emulates mouse wheel
 */
void do_mouse_wheel( int data )
{
	INPUT in;
	
	in.type = INPUT_MOUSE;
	in.mi.time = 0;
	in.mi.dwExtraInfo = ( ULONG_PTR )NULL;
	in.mi.dx = 0;
	in.mi.dy = 0;
	in.mi.mouseData = data;
	in.mi.dwFlags = MOUSEEVENTF_WHEEL;
	
	SendInput( 1, &in, sizeof(INPUT) );
}

/*
 * creates event_event object for mouse click event
 * and sends it to network event sender layer
 */
void handle_mouse_click ( int btn_click , int data)
{
	struct input_event *evt = (struct input_event *)malloc(sizeof(struct input_event));
	evt->type = INPUT_MOUSE_EVENT;
	evt->action = convert_to_mouse_net_event( btn_click );
	evt->mouse.x = 0;
	evt->mouse.y = 0;
    
    if( evt->action == EVENT_MOUSE_WHEEL )
    {
    	//little fix for wheel delta value
    	//why it does not set -120 from raw handler ?
    	//may be bcouse it uses unsigned data type for this value ?  
        evt->mouse.wheel = ( data == 65416 ? -WHEEL_DELTA : data );
    }

	send_remote_event(evt);
	
	free(evt);
}

int on_mouse_input_handler ( int dx, int dy, int flags, int data )
{

	update_screen_cursor ( dx, dy );

	if( is_on_remote_screen() )
	{
	    if( flags > 0 )
	    {
	        handle_mouse_click( flags, data );
	    }

	    return 1;
	}

	return 0;
}

void on_recv_mouse_event ( struct input_event *evt )
{
	if ( !evt )
	{
		return;
	}
	
	if ( evt->type == INPUT_MOUSE_EVENT )
	{
		if ( evt->action == EVENT_MOUSE_MOVE )
		{
			if ( IS_TEST_MODE )
			{
			    LOG_DEBUG( "received mouse event: move [ x=%d, y=%d ]", evt->mouse.x, evt->mouse.y );
			}
			else
			{
			    do_mouse_move( evt->mouse.x, evt->mouse.y );
			}
		}
		else
		{
			if ( IS_TEST_MODE )
			{
			    LOG_DEBUG( "received mouse event: click [ %s ]", get_action_name( evt->action ) );
			}
			if ( evt->action == EVENT_MOUSE_WHEEL )
			{
				do_mouse_wheel( evt->mouse.wheel );
			}
			else //its mouse click
			{
			    do_mouse_click( evt->action );
			}
		}
	}
}

void init_mouse ()
{
	register_mouse_handler( on_mouse_input_handler );
    
	if( register_event_receiver( INPUT_MOUSE_EVENT, on_recv_mouse_event ) == 0 )
    {
	    LOG_DEBUG("registered mouse event receiver");
	}	
}
