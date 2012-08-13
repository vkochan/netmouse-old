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
#include "log.h"
#include "winapi.h"

HWND create_event_window(char *name, wnd_proc wmain)
{
	WNDCLASSEX wc;
    MSG msg;
	HINSTANCE hInstance;
    HWND hwnd = NULL;
	
	hInstance = GetModuleHandle(NULL);

    // register window class
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = wmain;
    wc.hInstance     = hInstance;
    wc.lpszClassName = name;
     
    if( !RegisterClassEx( &wc ) )
    {
        LOG_FATAL("Window Registration Failed!");
        return NULL;
    }
    // create message-only window
    hwnd = CreateWindowEx(
        0,
        name,
        NULL,
        0,
        0, 0, 0, 0,
        HWND_MESSAGE, NULL, hInstance, NULL
    );
	
    if( !hwnd )
    {
        LOG_FATAL("Window Creation Failed!");
        return NULL;
    }
	
	return hwnd;
}

HWND create_window( char *name, char *title, int x, int y, int width, int heigth, int styles, wnd_proc wmain )
{
	HANDLE hInst = GetModuleHandle(NULL);
	
	WNDCLASSEX wClass;
	ZeroMemory(&wClass,sizeof(WNDCLASSEX));
	wClass.cbClsExtra=0;
	wClass.cbSize=sizeof(WNDCLASSEX);
	wClass.cbWndExtra=0;
	wClass.hbrBackground=(HBRUSH)COLOR_WINDOW;
	wClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	wClass.hIcon=NULL;
	wClass.hIconSm=NULL;
	wClass.hInstance=hInst;
	wClass.lpfnWndProc=(WNDPROC)wmain;
	wClass.lpszClassName=name;
	wClass.lpszMenuName=NULL;
	wClass.style=CS_HREDRAW|CS_VREDRAW;

	if( !RegisterClassEx( &wClass ) )
	{
		int errn = GetLastError();
		LOG_FATAL("Window class creation failed.Error = %d", errn);
		
		return NULL;
	}

	HWND hWnd = CreateWindowEx(0,
			name,
			title,
			styles,
			x,
			y,
			width,
			heigth,
			NULL,
			NULL,
			hInst,
			NULL);

	if( !hWnd )
	{
		int errn = GetLastError();
		LOG_FATAL( "Window Creation Failed.Error=%d",  errn );
		
		return NULL;
	}
	
	return hWnd;
}