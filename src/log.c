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
#include "log.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static log_handler_t logger = NULL;

void set_log_handler( log_handler_t log_handler )
{
    logger = log_handler;
}

void do_log( int log_type, char *fmt, ...  )
{	
	char msg[ 1000 ];
	int len = 0;
	
	va_list args;
    va_start( args, fmt );
	len = vsprintf( msg, fmt, args ); 
	va_end( args );
	
	if ( logger != NULL )
	{
	    logger( log_type, msg );
	}
	else
	{
	    printf( "%s\n", msg );
	}
}
