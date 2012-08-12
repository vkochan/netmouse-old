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
#include <shellapi.h>
#include <tchar.h>
#include <stdio.h>

#include "log.h"
#include "gui\logger.h"
#include "winapi.h"

#define WM_TRAYICON           ( WM_USER + 1 )
#define TASKBAR_ICON_APPID    ( WM_USER + 2 )

#define MNU_ID_TRAY_EXIT      ( WM_USER + 3 )
#define MNU_ID_TRAY_LOGS      ( WM_USER + 4 )
            
HWND wnd_taskbar;
HWND wnd_logger;

HMENU mnu_taskbar;
NOTIFYICONDATA notify_icon_data;

void create_icon_menu()
{
	mnu_taskbar = CreatePopupMenu();
	
	AppendMenu(mnu_taskbar, MF_STRING, MNU_ID_TRAY_LOGS,  TEXT( "Logs" ) );
	AppendMenu(mnu_taskbar, MF_STRING, MNU_ID_TRAY_EXIT,  TEXT( "Exit" ) );
};

void create_taskbar_icon(HWND hw)
{
    memset( &notify_icon_data, 0, sizeof( NOTIFYICONDATA ) ) ;
  
    notify_icon_data.cbSize = sizeof(NOTIFYICONDATA);
    notify_icon_data.hWnd = hw;
    notify_icon_data.uID = TASKBAR_ICON_APPID;
    notify_icon_data.uFlags = NIF_ICON | // promise that the hIcon member WILL BE A VALID ICON!!
      NIF_MESSAGE | // when someone clicks on the system tray icon,
      // we want a WM_ type message to be sent to our WNDPROC
      NIF_TIP;      // we're gonna provide a tooltip as well, son.

    notify_icon_data.uCallbackMessage = WM_TRAYICON; //this message must be handled in hwnd's window procedure. more info below.
    notify_icon_data.hIcon = (HICON)LoadImage( NULL, TEXT(".\\img\\netmouse.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE  ) ;
	
	Shell_NotifyIcon(NIM_ADD, &notify_icon_data);
}

int track_taskbar_menu( LPARAM lparam )
{
    if (lparam == WM_RBUTTONDOWN ) 
	{
	    POINT curPoint ;
		GetCursorPos( &curPoint ) ;

		SetForegroundWindow( wnd_taskbar ); 

		return TrackPopupMenu(
		  mnu_taskbar,
		  TPM_RETURNCMD | TPM_NONOTIFY,
		  curPoint.x,
		  curPoint.y,
		  0,
		  wnd_taskbar,
		  NULL
		);
	}
	
	return -1;
}

LRESULT CALLBACK taskbar_proc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
		case WM_CREATE:
		{  
		    wnd_taskbar = hwnd;
			
			create_icon_menu(); 
			create_taskbar_icon(hwnd);
			wnd_logger = create_logger_window();
		}
		break;
		
		case WM_TRAYICON:
		{           
			int mnu_item = track_taskbar_menu( lParam );
			
			if ( mnu_item == MNU_ID_TRAY_EXIT )
			{
			    PostQuitMessage( 0 );
			} 
			else if ( mnu_item == MNU_ID_TRAY_LOGS )
			{
			    ShowWindow( wnd_logger, TRUE );
			}
		}
		break;
    };
	
    return DefWindowProc( hwnd, message, wParam, lParam ) ;
}

void init_taskbar()
{
	wnd_taskbar = create_event_window( "NETMOUSE NOTIFY ICON WINDOW", taskbar_proc );
}

void cleanup_taskbar()
{
    Shell_NotifyIcon(NIM_DELETE, &notify_icon_data);
}