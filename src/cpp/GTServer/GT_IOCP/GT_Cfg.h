#pragma once

/* this file will be replace by JSON config file */

#define BUF_SIZE					4*1024
#define TCP_MODE_ENABLE				1
#define BIND_ADDRESS				"127.0.0.1"
#define BIND_PORT					7777
#define PRE_ALLOCATE_SOCKET_NUM		3000
#define REALLOCATE_SOKET_PER_SIZE	1000			// when the socket pool is not enough, we will allocate new socket,this is control size of sockets to be allocate
#define SIZEOF_USEFULL_SOCKET		50				// this is a value to control when to reallocate new size for pool, when the unuse socket size smaller than this will cause pool reallocate	