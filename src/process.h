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
 
#ifndef _PROCESS_HEADER_
#define _PROCESS_HEADER_

//#define PROCESS DWORD WINAPI

#define DECLARE_PROCESS(proc)\
		HANDLE proc##_process;\
		DWORD proc##_process_id;

#define RUN_PROCESS(proc, arg) \
		proc##_process = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(proc), (LPVOID)(arg), 0, \
			&proc##_process_id);

#endif