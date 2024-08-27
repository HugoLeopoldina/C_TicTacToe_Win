#ifndef _MYPOSIX_FUNCTIONS
#define _MYPOSIX_FUNCTIONS

#define UNICODE
#define _UNICODE

#ifdef __linux__
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <unistd.h> // getcwd
#include <memory.h>
#include <wchar.h>
#include <errno.h>

#define SOCKET int
#define SOCKET_ERROR -1
#define MAX_PATH PATH_MAX

#elif defined(_WIN32) || defined(_WIN64) ||\
	  defined(WIN32)  || defined(WIN64)
#define __windows__

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <direct.h>

// igualar para as funções do linux
#define getcwd _getcwd
#endif

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <locale.h>

#include "MyPOSIXStructs.h"

#define myfree(block) __myfree__((void**)block)

int _initialize(void);
int _finish(void);

int create_local_server(Server* server);
int get_ipv4_addr(char* host, IPV4_DATA* ipv4_data);
char* get_current_dir(void);

void freeServer(Server* server);
void __myfree__(void** block);

#endif
