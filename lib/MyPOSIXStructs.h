#ifndef _MYPOSIX_STRUCTS
#define _MYPOSIX_STRUCTS

#include "MyPOSIXFunctions.h"

#define _IN
#define _OUT

// Essa estrutura será usada para recuperar ou fornecer
// informações para um endereço de socket
typedef struct {
    _IN int port;
    _IN int cclient;
    _OUT SOCKET sockfd;
    _OUT char* ipv4_addr;
    _OUT char* hostname;
} Server, IPV4_ADDR;

#endif
