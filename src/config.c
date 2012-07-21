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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "config.h"
#include "log.h"
#include "types.h"

#define CFG_MAX_LINE 1024
#define CFG_DELIMITERS " \t="
#define CFG_FILE "netmouse.config"

#define EOL "\n"

char *server_addr = NULL;
char *server_port = NULL;
char *app_name = NULL;

struct screen_config screen_cfg;
bool _is_test_mode = FALSE;

bool is_test_mode()
{
    return _is_test_mode;
}

char *trim_str(char *str, char *trim_chars)
{
	char *s = strstr(str, trim_chars);
	
	if(s)
	{
		s[0] = '\0';
	}
	
	return str;
}

char *get_working_dir()
{
	char buff[MAX_PATH] = { 0 };
	char *dir = NULL;

	GetModuleFileNameA(NULL, buff, MAX_PATH);

	int len = strlen(buff);

	if (len <= 0)
	{
		return NULL;
	}

    int idx = len - 1;

	while(idx >= 0)
	{
		if (buff[idx] == '\\')
		{
			dir = (char *)malloc(idx + 1);
			ZeroMemory(dir, idx + 1);
			strncpy(dir, buff, idx);
			
			return dir;
		}

		idx--;
	}

	return NULL;
}

char *get_config_filepath()
{
	char *buff = NULL;
	char *curr_dir = NULL;
	char *config_file_path = NULL;
	int config_path_len = 0;

	curr_dir = get_working_dir();
	
	if (curr_dir == NULL)
	{
		return NULL;
	}

	config_path_len = strlen(curr_dir) + strlen(CFG_FILE) + 1;

	if (config_path_len <= 0)
	{
		return NULL;
	}

	config_file_path = (char *)malloc(config_path_len + 1);
	ZeroMemory(config_file_path, config_path_len + 1);
	
	config_file_path = strcat(config_file_path, curr_dir);
	config_file_path = strcat(config_file_path, "\\");
	config_file_path = strcat(config_file_path, CFG_FILE);
	
	return config_file_path;
}

/*
char *config_get_value(char *name)
{
	FILE *fd_config = NULL;
	char buff[CFG_MAX_LINE];
	char *key_token = NULL;
	char * config_path = NULL;

	config_path = get_config_filepath();

	if (config_path == NULL)
	{
		return NULL;
	}

	fd_config = fopen(config_path, "r");

	if (fd_config == NULL)
	{
		return NULL;
	}

	ZeroMemory(buff, CFG_MAX_LINE);

	while(fgets(buff, CFG_MAX_LINE, fd_config) != NULL)
	{
		key_token = strtok(buff, CFG_DELIMITERS);
		
		while(key_token != NULL)
		{
			if (strcmp(name, key_token) == 0)
			{
				char *val_token = strtok(NULL, CFG_DELIMITERS);

				if (val_token == NULL)
				{
					return NULL;
				}
				else
				{
					free(config_path);
					fclose(fd_config);
					
					val_token = trim_str(val_token, EOL);
					
					int val_len = strlen(val_token);
					char *ret_val = (char *)malloc(val_len + 1);
					ZeroMemory(ret_val, val_len + 1);
					strncpy(ret_val, val_token, val_len);
					
					return ret_val;
				}

				break;
			}

			key_token = strtok(NULL, CFG_DELIMITERS);
		}
	}

	free(config_path);
	fclose(fd_config);
	return NULL;
}

*/
char *config_get_value(FILE *f, char *name)
{
	FILE *fd_config = f;
	char buff[CFG_MAX_LINE];
	char *key_token = NULL;
	char * config_path = NULL;

	if (fd_config == NULL)
	{
		return NULL;
	}

	ZeroMemory(buff, CFG_MAX_LINE);

	while(fgets(buff, CFG_MAX_LINE, fd_config) != NULL)
	{
		key_token = strtok(buff, CFG_DELIMITERS);
		
		while(key_token != NULL)
		{
			if (strcmp(name, key_token) == 0)
			{
				char *val_token = strtok(NULL, CFG_DELIMITERS);

				if (val_token == NULL)
				{
					return NULL;
				}
				else
				{				
					val_token = trim_str(val_token, EOL);
					
					int val_len = strlen(val_token);
					char *ret_val = (char *)malloc(val_len + 1);
					ZeroMemory(ret_val, val_len + 1);
					strncpy(ret_val, val_token, val_len);
					
					return ret_val;
				}

				break;
			}

			key_token = strtok(NULL, CFG_DELIMITERS);
		}
	}
	
	return NULL;
}

int process_cmd_line(int argc, char **args)
{
	int idx = 1;
	char *srv_port = NULL;
	
	app_name = args[0];
	
	if (argc == 1)
	{
		return 1;
	}
	
	while(idx < argc)
	{	
		if( strcmp( args[idx], "-s" ) == 0 )
		{
			//check if its last argument
			if ( argc - idx == 1 )
			{
				LOG_ERROR("parameter [-s] requires address:port value\n");
				return 1;
			}
			
			server_addr = args[ ++idx ];
		}
		else if ( strcmp( args[ idx ], "-p" ) == 0 )
		{
			//check if its last argument
			if ( argc - idx == 1 )
			{
				LOG_ERROR("parameter [-s] requires address:port value\n");
				return 1;
			}
			
			server_port = args[ ++idx ];
		}
		else if ( strcmp (args[ idx ], "-t" ) == 0 )
		{
		    _is_test_mode = TRUE;
		}
		
		idx++;
	}
	
	return 0;
}

char *get_server_addr()
{
	return server_addr;
}

char *get_server_port()
{
	if (server_port == NULL)
	{
		return "12729";
	}
	
	return server_port;
}

char *get_app_name()
{
	return app_name;
}

int get_screen_config(struct screen_config *cfg)
{
	
	if (!cfg)
	{
	    return 1;
	}
	
	cfg->max_x = screen_cfg.max_x;
	cfg->max_y = screen_cfg.max_y;
	
	return 0;
}

void load_screen_config()
{
	memset( &screen_cfg, 0, sizeof(struct screen_config) );
	
	screen_cfg.max_x = GetSystemMetrics( SM_CXSCREEN );
	screen_cfg.max_y = GetSystemMetrics( SM_CYSCREEN );
}

void load_config_file()
{
	char *cfg_file_path = get_config_filepath();
	
	if (!cfg_file_path)
	{
		LOG_ERROR("Cant load netmouse.config file\n");
		return;
	}
	
	FILE *fd_config = fopen(cfg_file_path, "r");
	
	if (!cfg_file_path)
	{
		LOG_ERROR("Cant load netmouse.config file\n");
		return;
	}
	
	server_addr = config_get_value(fd_config, CFG_SERVER_ADDR);
	server_port = config_get_value(fd_config, CFG_SERVER_PORT);
	
	fclose(fd_config);
}

void load_config(int argc, char **args)
{
	process_cmd_line(argc, args);
	load_screen_config();
}
