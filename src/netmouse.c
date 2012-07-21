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

#include "event_pipe.h"
#include "screen.h"
#include "process.h"
#include "config.h"
#include "log.h"

DECLARE_PROCESS(main_app_loop);

/* 
 * processes windows message queue
 * in separated thread
 */ 
void message_loop()
{
	MSG message;
	while (GetMessage(&message,NULL,0,0)) {
		TranslateMessage( &message );
		DispatchMessage( &message );
	}
}

/* 
 * windows messages loop procedure
 */
DWORD WINAPI main_app_loop(LPVOID lpParm)
{	
	init_event_pipe();
	
	init_screen();
	
   //prevents event sending if its server ... 
   //may be its bad & it should be fixed
	if (get_server_addr())
	{
		init_input_handler();
	}
	
	init_mouse();
	init_keybd();
	
	message_loop();
	
	cleanup_input_handler();
	
	return 0;
}
 
void run_main_app(int argc, char **cmdl)
{	
	LOG_INFO("NetMouse by Vadim Kochan <vadim4j@gmail.com>\n\n");
	
	load_config(argc, cmdl);
	
	if ( IS_TEST_MODE )
	{
	    LOG_INFO("TEST MODE is ON\n");
	}
	
	LOG_INFO("SERVER: %s\n", get_server_addr());
	LOG_INFO("PORT: %s\n\n", get_server_port());
	
	RUN_PROCESS(main_app_loop, NULL);
	
	getchar();
}

int main(int argc, char **cmdLine)
{
	run_main_app(argc, cmdLine);
	return 0;
}