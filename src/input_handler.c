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
#include <stdio.h>

#include "mouse.h"
#include "keybd.h"
#include "log.h"
#include "config.h"
#include "types.h"
#include "input_handler.h"

#define BLOCK_INPUT() return 1

bool is_input_blocked = FALSE;
HHOOK hMouseHook;
HHOOK hKeybdHook;

mouse_handler_t on_mouse_handler = NULL;
keybd_handler_t on_keybd_handler = NULL;

void register_mouse_handler ( mouse_handler_t handler )
{
    if ( handler )
    {
        on_mouse_handler = handler;
    }
}

void register_keybd_handler ( keybd_handler_t handler )
{
    if ( handler )
    {
        on_keybd_handler = handler;
    }
}

int do_handle_mouse ( int dx, int dy, int flags )
{
    if ( on_mouse_handler )
    {
        return on_mouse_handler (dx, dy, flags );
    }
	
	return 0;
}

int do_handle_keybd (int vkey, int flags )
{
    if ( on_keybd_handler )
    {
        return on_keybd_handler ( vkey, flags );
    }
	
	return 0;
}

/*
 * thanks to this article http://www.rohitab.com/discuss/topic/35415-c-getrawinputdata-keylogger/
 */
LRESULT CALLBACK raw_input_device_handler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UINT dwSize;
    RAWINPUTDEVICE ridMouse;
    RAWINPUT *buffer;
     
    switch(msg)
    {
        case WM_CREATE:
            // Register a raw input devices to capture input
			
            ridMouse.usUsagePage = 0x01;
            ridMouse.usUsage = 0x02;
            ridMouse.dwFlags = RIDEV_INPUTSINK;
            ridMouse.hwndTarget = hwnd;
             
            if(!RegisterRawInputDevices(&ridMouse, 1, sizeof(RAWINPUTDEVICE)))
            {
                LOG_FATAL("Registering mouse raw input device failed!\n");
                return -1;
            }
			
        case WM_INPUT:
            // request size of the raw input buffer to dwSize
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize,
                sizeof(RAWINPUTHEADER));
         
            // allocate buffer for input data
            buffer = (RAWINPUT*)HeapAlloc(GetProcessHeap(), 0, dwSize);
         
            if(GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer, &dwSize,
                sizeof(RAWINPUTHEADER)))
            {
				if (buffer->header.dwType == RIM_TYPEMOUSE)
				{
				
					is_input_blocked = do_handle_mouse( buffer->data.mouse.lLastX, 
						buffer->data.mouse.lLastY, buffer->data.mouse.usButtonFlags);     
				}
            }
         
            // free the buffer
            HeapFree(GetProcessHeap(), 0, buffer);
            break;
             
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
             
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void register_raw_input_handler()
{
	WNDCLASSEX wc;
    HWND hwnd;
    MSG msg;
	HINSTANCE hInstance;
	char win_className[] = "NETMOUSE INPUT WINDOW HANDLER";

	hInstance = GetModuleHandle(NULL);

    // register window class
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = raw_input_device_handler;
    wc.hInstance     = hInstance;
    wc.lpszClassName = win_className;
     
    if(!RegisterClassEx(&wc))
    {
        LOG_FATAL("Window Registration Failed!\n");
        return ;
    }
    // create message-only window
    hwnd = CreateWindowEx(
        0,
        win_className,
        NULL,
        0,
        0, 0, 0, 0,
        HWND_MESSAGE, NULL, hInstance, NULL
    );
    if(!hwnd)
    {
        LOG_FATAL("Window Creation Failed!\n");
        return ;
    }
}

/*
 * yeah, may be its strange by I use different handling techniques for mouse & keybd
 * so, when I block input within keybd hook then I cant get events by RAW INPUT functions.
 * but for mouse I can get RAW INPUT events while input is blocked from mouse hook....
 */
__declspec(dllexport) LRESULT CALLBACK keybd_hook(int nCode, WPARAM wParam, LPARAM lParam)
{   
    KBDLLHOOKSTRUCT *keybdInfo = (KBDLLHOOKSTRUCT *)lParam;
	
	if (keybdInfo != NULL)
	{
		if ( do_handle_keybd( keybdInfo->vkCode, keybdInfo->flags ) )
		{
			BLOCK_INPUT();
		}
	}
	
	return CallNextHookEx( hKeybdHook, nCode, wParam, lParam );
}

/*
 * prevents mouse event receiving if our "virtual" mouse is on remote screen
 * but it enables of receving events from RAW INPUT layer.
 */
__declspec(dllexport) LRESULT CALLBACK mouse_hook(int nCode, WPARAM wParam, LPARAM lParam)
{
    MOUSEHOOKSTRUCT * pMouse = (MOUSEHOOKSTRUCT *)lParam;

	if (pMouse != NULL)
	{
		if( is_input_blocked )
		{
			BLOCK_INPUT();
		}
	}
    
    return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
} 

static void register_mouse_hook()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
	
    if (!hInstance)
    {
        hInstance = LoadLibrary((LPSTR)get_app_name());
    }
	 
    if (!hInstance)
	{
		return;
	}
 
    hKeybdHook = SetWindowsHookEx(  
        WH_MOUSE_LL,
        (HOOKPROC) mouse_hook,  
        hInstance,                 
        0                       
        );
}

static void unregister_mouse_hook()
{
    UnhookWindowsHookEx(hMouseHook);
}

static void register_keybd_hook()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
	
    if (!hInstance)
    {
        hInstance = LoadLibrary((LPSTR)get_app_name());
    }
	 
    if (!hInstance)
	{
		return;
	}
 
    hKeybdHook = SetWindowsHookEx(  
        WH_KEYBOARD_LL,
        (HOOKPROC) keybd_hook,  
        hInstance,                 
        0                       
        );
}

static void unregister_keybd_hook()
{
    UnhookWindowsHookEx(hKeybdHook);
}

static void register_hooks()
{
    register_mouse_hook();
    register_keybd_hook();
}

static void unregister_hooks()
{
    unregister_mouse_hook();
    unregister_keybd_hook();
}

/* initializes all hooks & input handlers */
void init_input_handler()
{	
    register_hooks();
    register_raw_input_handler();
}

void cleanup_input_handler()
{
    unregister_hooks();
}
