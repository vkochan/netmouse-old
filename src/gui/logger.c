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
#include <string.h>
#include <stdlib.h>

#include "log.h"
#include "types.h"

#define IDC_MAIN_BUTTON	101			// Button identifier
#define IDC_MAIN_EDIT	102			// Edit box identifier

HWND logs_box = NULL;

#define MAX_LOG_SIZE 1024*100

int log_len = 0;
char log_buffer[ MAX_LOG_SIZE ] = { 0 };

/*
 * fills the log buffer & shows it when the log window is visible
 */
void gui_logger (int log_level, char *msg)
{	
	int len = strlen( msg );
	
	if( ( log_len +  len + 2 ) >= MAX_LOG_SIZE )
	{
	    memset( log_buffer, 0, MAX_LOG_SIZE );
		log_len = 0;
	}
	
	strcat( log_buffer, msg );
	strcat( log_buffer, "\r\n" );
	
	log_len += len + 1;
}

BOOL CALLBACK logger_main( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch(msg)
	{
		case WM_CREATE:
		{
			logs_box = CreateWindowEx(WS_EX_CLIENTEDGE,
				"EDIT",
				"",
				WS_CHILD|WS_VISIBLE|
				ES_MULTILINE|ES_AUTOVSCROLL|ES_AUTOHSCROLL,
				10,
				10,
				600,
				430,
				hwnd,
				(HMENU)IDC_MAIN_EDIT,
				GetModuleHandle(NULL),
				NULL);
			
			EnableMenuItem( GetSystemMenu( hwnd, FALSE), SC_CLOSE, MF_ENABLED );
		}
		break;
        
		case WM_CLOSE:
			ShowWindow( hwnd, FALSE );
			return 0;
			
		case WM_ACTIVATE:
				SendMessage(logs_box, WM_SETTEXT , NULL, (LPARAM)log_buffer );
		break;
	}

	return DefWindowProc(hwnd,msg,wParam,lParam);
}

HWND create_logger_window()
{			
    HWND hwnd = create_window( "LOGGER WINDOW", "Logs", 200, 200, 640, 480, WS_CAPTION | WS_SYSMENU, logger_main );
	
	if(!hwnd)
	{
		int errn = GetLastError();

		LOG_FATAL( "Window Creation Failed.Error=%d",  errn );
	}
	
	return hwnd;
}

