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
#include "types.h"

#include <assert.h>
#include <windows.h>
#include <stdlib.h>

#include "slab.h"

/*
 * initializes slab allocator
 * allocates piece of memory for caching
 * slab objects
 */
struct slab *alloc_slab( int obj_size, int max_count )
{
    struct slab *sl = (struct slab *)malloc( sizeof( struct slab ) );

    assert( sl != NULL );

    memset( sl, 0, sizeof( struct slab ) );

    sl->obj_size = obj_size;
    sl->max_count = max_count;
    sl->cache = malloc( obj_size * max_count );
}
