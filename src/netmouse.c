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
#include <tchar.h>

#include "event_pipe.h"
#include "screen.h"
#include "config.h"
#include "log.h"

#ifdef _GUI_
	#include "gui\taskbar.h"
	#include "gui\logger.h"
#endif

/* 
 * processes windows message queue
 * in separated thread
 */ 
void message_loop()
{
	MSG message;
	while ( GetMessage( &message,NULL,0,0 ) ) 
	{
		TranslateMessage( &message );
		DispatchMessage( &message );
	}
}

void init_modules()
{	
	init_event_pipe();
	
	init_screen();
	
   //prevents event sending if its server ... 
   //may be its bad & it should be fixed
	if ( get_server_addr() )
	{
		init_input_handler();
	}
	
	init_mouse();
	init_keybd();
	
	#ifdef _GUI_
	    init_taskbar();
	#endif
}

void cleanup_modules()
{
    cleanup_taskbar();
    cleanup_input_handler();
}

void run_main_app_loop()
{	
    init_modules();
	message_loop();
	cleanup_modules();
}
 
void run_main_app( )
{	
	#ifdef _GUI_
	    set_log_handler( gui_logger );
	#endif
	
	LOG_INFO("NetMouse by Vadim Kochan <vadim4j@gmail.com>\n\r");
	
	load_config();
	
	if ( IS_TEST_MODE )
	{
	    LOG_INFO( "TEST MODE is ON" );
	}
	
	LOG_INFO( "SERVER: %s", get_server_addr() );
	LOG_INFO( "PORT: %s\n\r", get_server_port() );
	
	run_main_app_loop();
}

#ifdef _GUI_

	int APIENTRY WinMain(HINSTANCE h_inst, HINSTANCE h_prev_inst, LPSTR cmd_line, int cmd_show)
	{	
		run_main_app();
		return 0;
	}

#else
	int main(int argc, char ** argv)
	{
	    process_cmd_line( argc, argv );
		run_main_app();
	}
#endif