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

#ifndef _SLAB_HEADER_
#define _SLAB_HEADER_

struct slab
{
    int obj_size;
    int max_count;
    void *slab_buff;
};

struct slab_obj
{
	int idx;
	void *obj;
	bool is_used;
};

struct slab *alloc_slab( int obj_size, int max_count );
void free_slab( struct slab *sl );

struct slab_obj *slab_alloc_obj( struct slab *sl );
void  slab_free_obj( struct slab_obj *obj );

#endif