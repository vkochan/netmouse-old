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
 
#include <windows.h>
#include <stdio.h>

#include "event_pipe.h"
#include "event_receiver.h"
#include "event_sender.h"
#include "process.h"
#include "config.h"
#include "log.h"

DECLARE_PROCESS(app_loop);

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
DWORD WINAPI app_loop(LPVOID lpParm)
{	
	init_event_pipe();
	
	init_event_receiver();
	
	init_event_sender();
	
   //prevents event sending if its server ... 
   //may be its bad & it should be fixed
	if (get_server_addr())
	{
		init_input_handler();
	}
	
	message_loop();
	
	cleanup_input_handler();
	
	return 0;
}
 
/* 
 * should be ran as deamonized service
 */
void start_app_service(int argc, char **cmdl)
{	
	LOG_INFO("NetMouse by Vadim Kochan <vadim4j@gmail.com>\n\n");
	
	load_config(argc, cmdl);
	
	LOG_INFO("server=%s\n", get_server_addr());
	LOG_INFO("port=%s\n\n", get_server_port());
	
	RUN_PROCESS(app_loop, NULL);
	
	getchar();
}
 
 