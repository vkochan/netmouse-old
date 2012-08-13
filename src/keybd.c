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
#include <assert.h>
 
#include "input_handler.h"
#include "screen.h"
#include "log.h"
#include "config.h"

#define KEYBD_PRESS_UP   0
#define KEYBD_PRESS_DOWN 1
				 
int is_keyup(int flags)
{
	return flags & ( 1 << 7);
}
 
void do_keybd_press(int action, int vkey, int flags)
{
	//unsigned short int scanCode = (unsigned short int)MapVirtualKey(vkey, 0);
	
	INPUT in;
	in.type = INPUT_KEYBOARD;
	in.ki.wVk = vkey;
	in.ki.time = 0;
	
	if(action == KEYBD_PRESS_DOWN) //key is down
	{
		in.ki.dwFlags = 0;
		in.ki.wScan = 0;//(unsigned short int)(scanCode & 0xff);
	}
	else if (action == KEYBD_PRESS_UP) //key is up
	{
		in.ki.dwFlags = KEYEVENTF_KEYUP;
		in.ki.wScan = 0;//scanCode;
	}
	
	SendInput(1, &in, sizeof(INPUT));
}

int on_keybd_input_handler(int vkey, int flags)
{
	if ( !is_on_remote_screen() )
	{
	    return 0;
	}
	
	struct input_event *evt = (struct input_event *)malloc(sizeof(struct input_event));
	
	if (!evt)
	{
		return 0;
	}
	
	evt->type = INPUT_KEYBD_EVENT;
	evt->action = is_keyup( flags ) ? KEYBD_PRESS_UP : KEYBD_PRESS_DOWN;
	evt->keybd.vkey = vkey;
	evt->keybd.flags = 0;
	
	send_remote_event(evt);
	
	return 1;
}

void log_keybd_event( struct input_event *evt )
{
    assert( evt != NULL );

    char key_name[ 32 ] = { 0 };
	char *action_name = NULL;

    if ( evt->action == KEYBD_PRESS_DOWN )
    {
	    action_name = "PRESSED UP";
	}
	else if ( evt->action == KEYBD_PRESS_UP )
	{
	    action_name = "PRESSED DOWN";
	}
	
	GetKeyNameText(MAKELONG(0, MapVirtualKey(evt->keybd.vkey, 0)), key_name, 32);
	
	LOG_DEBUG("received keybd event [ %s %s ]", key_name, action_name );
}

void on_recv_keybd_event ( struct input_event *evt )
{
    if ( !evt )
	{
		LOG_ERROR("received empty event");
	}
	
	if ( IS_TEST_MODE )
	{
	    log_keybd_event( evt );
	}
	else
	{
	    do_keybd_press( evt->action, evt->keybd.vkey,  evt->keybd.flags );
	}
}

void init_keybd()
{
    register_keybd_handler( on_keybd_input_handler );
	
	if( register_event_receiver( INPUT_KEYBD_EVENT, on_recv_keybd_event ) == 0 )
    {
	    LOG_DEBUG("registered keyboard event receiver");
	}
}