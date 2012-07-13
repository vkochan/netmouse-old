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
#include <stdio.h>
#include <string.h>

#include "mouse.h"
#include "event_sender.h"
#include "log.h"

/*
 * converts windows raw mouse event type into custom type
 */
int convert_to_mouse_net_event(int event_val)
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
int convert_net_to_mouse_event(int net_event)
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

/*
 * emulates mouses move by absolute coordinates
 */
void do_move_mouse(int x, int y)
{
	INPUT in;
	
	in.type = INPUT_MOUSE;
	in.mi.time = 0;
	in.mi.dwExtraInfo = (ULONG_PTR)NULL;
	in.mi.dx = x;
	in.mi.dy = y;
	in.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
	
	SendInput(1, &in, sizeof(INPUT));
}

/*
 * emulates mouse click event
 */
void do_mouse_click(int btn_click)
{
	INPUT in;
	
	in.type = INPUT_MOUSE;
	in.mi.time = 0;
	in.mi.dwExtraInfo = (ULONG_PTR)NULL;
	in.mi.dx = 0;
	in.mi.dy = 0;
	in.mi.dwFlags = convert_net_to_mouse_event(btn_click);
	
	SendInput(1, &in, sizeof(INPUT));
}

/*
 * creates event_event object for mouse move event
 * and sends it to network event sender layer
 */
void handle_mouse_move(int x, int y)
{	
	struct input_event *evt = (struct input_event *)malloc(sizeof(struct input_event));
	evt->type = INPUT_MOUSE_EVENT;
	evt->action = EVENT_MOUSE_MOVE;
	evt->point.x = x;
	evt->point.y = y;
	
	send_event(evt);
	
	free(evt);
}

/*
 * creates event_event object for mouse click event
 * and sends it to network event sender layer
 */
void handle_mouse_click(int btn_click)
{
	struct input_event *evt = (struct input_event *)malloc(sizeof(struct input_event));
	evt->type = INPUT_MOUSE_EVENT;
	evt->action = convert_to_mouse_net_event(btn_click);
	evt->point.x = 0;
	evt->point.y = 0;

	send_event(evt);
	
	free(evt);
}


