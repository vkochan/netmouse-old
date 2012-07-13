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
#include <stdio.h>

#include "mouse.h"
#include "keybd.h"
#include "log.h"
#include "config.h"

#define BLOCK_INPUT() return 1

#define NORMAL_SCREEN -1
#define LEFT_SCREEN    0
#define RIGHT_SCREEN   1

HHOOK hMouseHook;
HHOOK hKeybdHook;

int maxX;
int maxY;
int virt_x = 0;
int virt_y = 0;

int curr_scr = -1;

//HACK ... its needed by event_pipe module to unblock
//input events for apps if we lost connection with server
void disable_remote_events()
{
	curr_scr = NORMAL_SCREEN;
}

/*
 * hides mouse cursor when it touches left or right screen side
 */
void hide_mouse_cursor()
{
	POINT cursor_pos;
	GetCursorPos(&cursor_pos);
	SetCursorPos(maxX - 1, cursor_pos.y);
}

/*
 * shows mouse cursor when virtual x coord (virt_x) 
 * is between 0 & maxX (max screen width)
 */
void show_mouse_cursor(int screen_type)
{
	POINT cursor_pos;
	
	if (screen_type == LEFT_SCREEN)
	{
		GetCursorPos(&cursor_pos);
		SetCursorPos(1, cursor_pos.y);
	}
	else if (screen_type == RIGHT_SCREEN)
	{
		GetCursorPos(&cursor_pos);
		SetCursorPos(maxX-2, cursor_pos.y);
	}
}

/*
 * checks if mouse is on one of the remote screen (right or left)
 */
bool is_on_remote_screen()
{
	return (curr_scr == RIGHT_SCREEN || curr_scr == LEFT_SCREEN);
}

/*
 * calculates the screen side which cursor has touched
 */
void calc_selected_screen()
{
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	
	if (curr_scr == NORMAL_SCREEN)
	{
		if (cursorPos.x >= maxX - 1)
		{
			curr_scr = RIGHT_SCREEN;
		}
		else if (cursorPos.x <= 0)
		{
			curr_scr = LEFT_SCREEN;
		}
	}
}

/*
 * updates virtual coordinates (virt_x & virt_y)
 * dx & dy are relatave values on which cursor was moved
 * calculation depends at which screen side we are
 */
void calc_virt_coord(int dx, int dy)
{
	calc_selected_screen();
	
	if (curr_scr != NORMAL_SCREEN)
	{
		virt_x += dx;
		virt_y += dy;
	}
	
	if (curr_scr == RIGHT_SCREEN)
	{	
		if (virt_x < 0)
		{
			virt_x = 0;
		}
		
		if (virt_y < 0)
		{
			virt_y = 0;
		}
		
		if (virt_x <= 0)
		{
			curr_scr = NORMAL_SCREEN;
			show_mouse_cursor(RIGHT_SCREEN);
		}
	}
	else if (curr_scr == LEFT_SCREEN)
	{	
		if (virt_x >= 0)
		{
			virt_x = 0;
		}
		
		if (virt_y >= 0)
		{
			virt_y = 0;
		}
		
		if (virt_x >= 0)
		{
			curr_scr = NORMAL_SCREEN;
			show_mouse_cursor(LEFT_SCREEN);
		}
	}
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
					calc_virt_coord(buffer->data.mouse.lLastX, buffer->data.mouse.lLastY);
						
					if (is_on_remote_screen())
					{
						if (buffer->data.mouse.lLastX && buffer->data.mouse.lLastY)
						{
							hide_mouse_cursor();
							handle_mouse_move(virt_x, virt_y);
						}
						
						if (buffer->data.mouse.usButtonFlags > 0)
						{
							handle_mouse_click(buffer->data.mouse.usButtonFlags);
						}
					}
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
		if (is_on_remote_screen())
		{
			handle_keybd_action(keybdInfo->vkCode, keybdInfo->flags);
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
		if(is_on_remote_screen())
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

void load_screen_info()
{
	maxX = GetSystemMetrics(SM_CXSCREEN);
	maxY = GetSystemMetrics(SM_CYSCREEN);
}

/* initializes all hooks & input handlers */
void init_input_handler()
{	
	load_screen_info();
	register_hooks();
	register_raw_input_handler();
}

void cleanup_input_handler()
{
	unregister_hooks();
}