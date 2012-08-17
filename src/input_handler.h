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
 
#ifndef _INPUT_HANDLER_HEADER_
#define _INPUT_HANDLER_HEADER_

typedef int ( * mouse_handler_t ) ( int dx, int dy, int flags, int data );
typedef int ( * keybd_handler_t ) ( int vcode, int flags ) ;
 
void init_input_handler();

void cleanup_input_handlers();

void disable_remote_events();

void register_mouse_handler ( mouse_handler_t handler );

void register_keybd_handler ( keybd_handler_t handler );

#endif
