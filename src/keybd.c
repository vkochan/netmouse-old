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

#include "defs.h"

#include <windows.h>
#include <stdlib.h>

#include "event_sender.h"
#include "log.h"

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

int handle_keybd_action(int vkey, int flags)
{
	struct input_event *evt = (struct input_event *)malloc(sizeof(struct input_event));
	
	if (!evt)
	{
		return -1;
	}
	
	evt->type = INPUT_KEYBD_EVENT;
	evt->action = is_keyup(flags) ? KEYBD_PRESS_UP : KEYBD_PRESS_DOWN;
	evt->keybd.vkey = vkey;
	evt->keybd.flags = 0;
	
	send_event(evt);
}