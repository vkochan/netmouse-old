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

#ifndef _WINAPI_HEADER_
#define _WINAPI_HEADER_

typedef LRESULT CALLBACK (* wnd_proc )( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

HWND create_event_window( char *name, wnd_proc wmain );

HWND create_window( char *name, char *title, int x, int y, int width, int heigth, int styles, wnd_proc wmain );

#endif