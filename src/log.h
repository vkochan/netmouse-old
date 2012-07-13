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
 
#ifndef _LOG_HEADER_
#define _LOG_HEADER_

#define DEBUG 1
							
#define LOG_DEBUG(fmt, ...) \
	do { if (DEBUG) printf("[DEBUG] %s:%d:%s(): " fmt,__FILE__ \
							,__LINE__ ,__FUNCTION__,  ## __VA_ARGS__); } while (0)

#define LOG_ERROR(fmt, ...) printf("[ERROR] " fmt, ## __VA_ARGS__)
#define LOG_WARN(fmt, ...) printf("[WARN] " fmt, ## __VA_ARGS__)
#define LOG_FATAL(fmt, ...) printf("[FATAL] " fmt, ## __VA_ARGS__)
#define LOG_INFO(fmt, ...) printf("[INFO] " fmt, ## __VA_ARGS__)

#endif