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
 
#ifndef _INPUT_EVENT_HEADER_
#define _INPUT_EVENT_HEADER_

#define INPUT_MOUSE_EVENT				0
#define INPUT_KEYBD_EVENT				1
#define INPUT_DATA_EVENT                2

struct mouse
{
    int x;
	int y;
    int wheel;
};

struct keybd
{
	int vkey;
	int flags;
};

struct input_event
{
	int type;		//mouse or keyboard
	int action;	//press, unpress, etc, specific for event type
	
	union
	{
		struct mouse mouse;
		struct keybd keybd;
	};
};

#endif