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

#include "list.h"
#include <stdlib.h>

struct list *create_list()
{
    return ( struct list * )malloc( sizeof( struct list ) );
}

void free_list( struct list *l )
{
    if ( l )
	{	
		free( l );
	}
}

/*
 * adds element with pointer to data in linked list
 * return an index newly created list item
 */
int list_add( struct list *l, void *obj )
{
    if( !l || !obj )
	{
	    return -1;
	}
	
	if ( !( l->head ) )
	{
	    l->head = ( struct list_item * )malloc( sizeof( struct list_item ) );
		
		if ( !( l->head ) )
		{
		    return -1;
		}
		
		memset( l->head, 0, sizeof( struct list ) );
		
		l->head->prev = l->head;
		l->head->next = l->head;
		l->head->hlist = l;
		l->head->obj = obj;
		
		l->count++;
		
		return ( l->count ) - 1;
	}
	
	struct list_item *item = ( struct list_item * )malloc( sizeof( struct list_item ) );
	
	if ( !item )
	{
	    return -1;
	}
	
	memset( item, 0, sizeof( struct list_item ) );
	
	//makes new item as head element of list
	item->obj = obj;
	item->hlist = l;
	item->next = l->head->next;
	item->prev = l->head->prev;
	
	l->head->prev = item;
	l->head = item;
    l->count++;

    return ( l->count ) - 1;	
}