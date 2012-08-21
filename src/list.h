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

#ifndef _LIST_HEADER_
#define _LIST_HEADER_

struct list_head;
struct list_item;

struct list
{
    struct list_item *head;
    int count;    
};

struct list_item
{
    struct list_item *next;
	struct list_item *prev;
	void *obj;
	struct list *hlist; 
};

struct list *create_list();
void free_list( struct list *l );

int list_add( struct list *l, void *obj );
void list_del( struct list *l, void *obj );
void list_del_at( struct list *l, int idx );

//void list_push( struct list *l, void *data );
//void list_pop (struct list *l, void *data );

#endif