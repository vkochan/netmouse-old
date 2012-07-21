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

#include "types.h"
#include "input_event.h"

#ifndef _REMOTE_SCREEN_HEADER_
#define _REMOTE_SCREEN_HEADER_

//screen orientation

#define NORMAL_SCREEN -1
#define LEFT_SCREEN    0
#define RIGHT_SCREEN   1
#define SHARED_SCREEN  2

//remote screen statuses
#define REMOTE_SCREEN_CONNECTED    0
#define REMOTE_SCREEN_RECEIVING    1
#define REMOTE_SCREEN_SENDING      2
#define REMOTE_SCREEN_INIT         3
#define REMOTE_SCREEN_ACTIVE       4
#define REMOTE_SCREEN_PASSIVE      5
#define REMOTE_SCREEN_DISCONNECTED 6
#define REMOTE_SCREEN_CONNECTING   7
#define REMOTE_SCREEN_WAITING      8

struct screen
{
    int type;                    //  client or server
    int orient;                  //  left or right or combined left | right
    int status;                  // connected, disconnected, active, inactive
    struct event_pipe *evt_pipe;
    char *addr;
    char *port;
};

struct screen *get_remote_screen();
struct screen *get_shared_screen();

void update_screen_cursor( int dx, int dy );

int setup_remote_screen( char *addr, char *port, int orient );

int setup_shared_screen ( char *port );

int send_event ( struct input_event *evt );

void init_screen ();

bool is_on_remote_screen ();

int register_event_receiver( int evt_type, void (* on_recv_event) ( struct input_event * ) );

int unregister_event_receiver( int evt_type );

#endif
