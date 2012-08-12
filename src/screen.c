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
#include "screen.h"
#include "input_event.h"
#include "event_pipe.h"
#include "config.h"
#include "mouse.h"
#include "log.h"

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

int virt_x = 0;
int virt_y = 0;

static int curr_scr = NORMAL_SCREEN;
static struct screen* screen_array[3] = { 0 };

#define MAX_X (GetSystemMetrics( SM_CXSCREEN ))
#define MAX_Y (GetSystemMetrics( SM_CYSCREEN ))

#define MAX_EVENT_RECEIVERS 2

void (* event_receivers[ MAX_EVENT_RECEIVERS ]) ( struct input_event * ) = { 0 };

struct screen *get_remote_screen()
{
    if ( curr_scr == LEFT_SCREEN || curr_scr == RIGHT_SCREEN )
	{
	    return screen_array[ curr_scr ];
	}
	
	return NULL;
}

struct screen *get_shared_screen()
{
    return screen_array[ SHARED_SCREEN ];
}

int register_event_receiver( int evt_type, void (* on_recv_event) ( struct input_event * ) )
{
    if ( evt_type < 0 || evt_type > MAX_EVENT_RECEIVERS )
	{
	    return 1;
	}
	
	event_receivers[ evt_type ] = on_recv_event;
	return 0;
}

int unregister_event_receiver( int evt_type )
{
    if ( evt_type < 0 || evt_type > MAX_EVENT_RECEIVERS )
	{
	    return -1;
	}
	
    event_receivers[ evt_type ] = NULL;
	return 0;
}

void do_event_receiving( struct input_event *evt )
{	
	int i = 0;
	
	if ( !evt )
	{
	    LOG_ERROR("received empty event");
		return;
	}
	
	do
	{
	    if ( event_receivers[ i ] && ( i == evt->type ) )
		{
		    event_receivers[ i ]( evt );
			return;
		}
	} while ( ++i < MAX_EVENT_RECEIVERS );
}

//HACK ... its needed by event_pipe module to unblock
//input events for apps if we lost connection with server
void disable_remote_events()
{
	curr_scr = NORMAL_SCREEN;
}

/*
 * hides mouse cursor when it touches left or right screen side
 */
void hide_mouse_cursor()
{
	POINT cursor_pos;
	GetCursorPos(&cursor_pos);
	SetCursorPos(MAX_X - 1, cursor_pos.y);
}

/*
 * shows mouse cursor when virtual x coord (virt_x) 
 * is between 0 & MAX_X (max screen width)
 */
void show_mouse_cursor(int screen_type)
{
	POINT cursor_pos;
	
	if (screen_type == LEFT_SCREEN)
	{
		GetCursorPos( &cursor_pos );
		SetCursorPos( 1, cursor_pos.y );
	}
	else if ( screen_type == RIGHT_SCREEN )
	{
		GetCursorPos( &cursor_pos );
		SetCursorPos( MAX_X - 2, cursor_pos.y );
	}
}

/*
 * checks if mouse is on one of the remote screen (right or left)
 */
bool is_on_remote_screen()
{
	return ( curr_scr == RIGHT_SCREEN || curr_scr == LEFT_SCREEN );
}

/*
 * calculates the screen side which cursor has touched
 */
static void calc_selected_screen()
{
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	
	if (curr_scr == NORMAL_SCREEN)
	{
		if (cursorPos.x >= MAX_X - 1)
		{
			curr_scr = RIGHT_SCREEN;
		}
		else if (cursorPos.x <= 0)
		{
			curr_scr = LEFT_SCREEN;
		}
	}
}

/*
 * updates virtual coordinates (virt_x & virt_y)
 * dx & dy are relatave values on which cursor was moved
 * calculation depends at which screen side we are
 */
static void calc_virt_coord(int dx, int dy)
{
	calc_selected_screen();
	
	if (curr_scr != NORMAL_SCREEN)
	{
		virt_x += dx;
		virt_y += dy;
	}
	
	if (curr_scr == RIGHT_SCREEN)
	{	
		if (virt_x < 0)
		{
			virt_x = 0;
		}
		
		if (virt_y < 0)
		{
			virt_y = 0;
		}
		
		if (virt_x <= 0)
		{
			curr_scr = NORMAL_SCREEN;
			show_mouse_cursor(RIGHT_SCREEN);
		}
	}
	else if (curr_scr == LEFT_SCREEN)
	{	
		if (virt_x >= 0)
		{
			virt_x = 0;
		}
		
		if (virt_y >= 0)
		{
			virt_y = 0;
		}
		
		if (virt_x >= 0)
		{
			curr_scr = NORMAL_SCREEN;
			show_mouse_cursor(LEFT_SCREEN);
		}
	}
}

static struct screen *clone_screen ( struct screen *scr )
{
	if ( !scr )
	{
		return NULL;
	}

	struct screen *to_scr = ( struct screen * ) malloc( sizeof ( struct screen ) );

	if ( !to_scr )
	{
		return NULL;
	}
    
	to_scr->type = scr->type;
	to_scr->evt_pipe = scr->evt_pipe;
	to_scr->orient = scr->orient;
	to_scr->status = scr->status;
	to_scr->addr = scr->addr;
	to_scr->port = scr->port;
	
    return to_scr;
}

static void on_connection_changed ( struct event_pipe *evt_pipe )
{	
	if ( !evt_pipe )
	    return;
	
	if ( !evt_pipe->obj )
	    return;
		
	struct screen *scr = ( struct screen * )evt_pipe->obj;
	
	if ( !scr )
	{
	    return;
	}
	
	switch ( evt_pipe->conn_state )  
	{
       
		case EVENT_PIPE_CONNECTED :
			scr->status = REMOTE_SCREEN_CONNECTED;
			LOG_INFO("Connected to remote screen");
			break;

		case EVENT_PIPE_DISCONNECTED :
			scr->status = REMOTE_SCREEN_DISCONNECTED;
			break;

		case EVENT_PIPE_CONNECTING :
			scr->status = REMOTE_SCREEN_CONNECTING;
			break;
			
		case EVENT_PIPE_LISTENNING :
			scr->status = REMOTE_SCREEN_WAITING;
			LOG_INFO("Waiting for events ...");
			break;
			
        case EVENT_PIPE_ACCEPTED :
		    scr->status = REMOTE_SCREEN_CONNECTED;
			LOG_INFO("Connected to sending event pipe");
			break;
	}
}

int setup_remote_screen ( char *addr, char *port, int orient )
{
	struct event_pipe *evt_pipe = create_event_pipe( addr, port, EVENT_PIPE_SEND );
	struct screen *remote_scr;
    
	if ( !evt_pipe )
	{
		return -1;
	}
    
	remote_scr = ( struct screen *)malloc( sizeof( struct screen ) );

	if ( !remote_scr )
	{
		return -1;
	}
    
	evt_pipe->on_conn_state = on_connection_changed;
	evt_pipe->obj = remote_scr;
	
	memset( remote_scr, 0, sizeof( struct screen ) ); 

	remote_scr->evt_pipe = evt_pipe;
	remote_scr->orient = orient;
	remote_scr->status = REMOTE_SCREEN_INIT;
	remote_scr->addr = addr;
	remote_scr->port = port;
    
    
	if ( orient == LEFT_SCREEN || orient == RIGHT_SCREEN )
	{
		screen_array[ orient ] = remote_scr; 
	}
	else if ( orient == ( LEFT_SCREEN | RIGHT_SCREEN ) )
	{
		screen_array[ LEFT_SCREEN ] = remote_scr;
		screen_array[ RIGHT_SCREEN ] = clone_screen( remote_scr );
    }

	open_event_pipe( evt_pipe );
	
    return 0;
}



int setup_shared_screen ( char *port )
{
	struct event_pipe *evt_pipe = create_event_pipe( NULL, port, EVENT_PIPE_RECV );
    struct screen *shrd_screen;
	
	if ( !evt_pipe )
	{
        	return -1;
	}
    
	shrd_screen = ( struct screen *)malloc( sizeof( struct screen ) );

	if ( !shrd_screen )
	{
        	return -1;
	}

	evt_pipe->on_recv_event = do_event_receiving;
	evt_pipe->on_conn_state = on_connection_changed;
	evt_pipe->obj = shrd_screen;
	
	memset( shrd_screen, 0, sizeof( struct screen ) );

	shrd_screen->evt_pipe = evt_pipe;
	shrd_screen->status = REMOTE_SCREEN_INIT;
	shrd_screen->addr = NULL;
	shrd_screen->port = port;
	shrd_screen->orient = SHARED_SCREEN;
	
	LOG_DEBUG("opening shared screen ...");
	
	open_event_pipe( evt_pipe );
	
    return 0;
}

int send_remote_event ( struct input_event *evt )
{
	if ( curr_scr != NORMAL_SCREEN && evt )
	{
		if ( screen_array[ curr_scr ] != NULL )
		{
			return do_send_event_pipe( screen_array[ curr_scr ]->evt_pipe, evt ) ;
		}
	}

	return -1;
}

void send_cursor_move( int x, int y )
{
	struct input_event *evt = (struct input_event *)malloc(sizeof(struct input_event));
	evt->type = INPUT_MOUSE_EVENT;
	evt->action = EVENT_MOUSE_MOVE;
	evt->point.x = x;
	evt->point.y = y;
	
	send_remote_event(evt);
	
	free(evt);
}

void update_screen_cursor( int dx, int dy )
{
	calc_virt_coord( dx, dy);
	
	if ( is_on_remote_screen() )
	{
		send_cursor_move( virt_x, virt_y );
	}
}

void init_screen()
{
    if ( !get_server_addr() )
	{
	    LOG_DEBUG("setup shared screen ...");
		setup_shared_screen( get_server_port() );
	}
	else
	{
	    setup_remote_screen( get_server_addr(), get_server_port() , RIGHT_SCREEN );
	}
}
