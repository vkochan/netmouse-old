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
 
#ifndef _MOUSE_HEADER_
#define _MOUSE_HEADER_
 
#include "types.h"

#define EVENT_MOUSE_MOVE				0
#define EVENT_MOUSE_LEFT_DOWN			1
#define EVENT_MOUSE_LEFT_UP			    2
#define EVENT_MOUSE_MIDDLE_DOWN		    3
#define EVENT_MOUSE_MIDDLE_UP			4
#define EVENT_MOUSE_RIGHT_DOWN		    5
#define EVENT_MOUSE_RIGHT_UP			6
#define EVENT_MOUSE_WHEEL				7

void handle_mouse_move(int x, int y);

void handle_mouse_click(int btn_click);

bool is_mouse_on_remote();

void do_move_mouse(int x, int y);

void do_mouse_click(int btn_click);

void init_mouse();

#endif