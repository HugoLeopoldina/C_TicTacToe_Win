#define _UNICODE
#define UNICODE

// Servidor responsável por conectar 2 clientes
// e gerenciar o jogo. Funcionará tanto para windows
// quanto para linux.

// Inclusão para linux
#ifdef __linux__
#include <unistd.h>
#include <sys/socket.h>

#define SOCKET int

// Inclusão para windows
#elif defined(_WIN32) || defined(_WIN64) ||\
      defined(WIN32)  || defined(WIN64)
#define __windows__
#include <WinSock2.h>
#include <WS2tcpip.h> // getaddrinfo

#pragma comment(lib, "ws2_32.lib")

#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <locale.h>

// no linux a função close fecha um descritor de arquivo,
// sendo ele um socket. No windows, a função que faz a mesma operação
// é o closesocket, define apenas para manter a compatibilidade
#define close closesocket

int get_ipv4_addr(char* buffer, int len);

int main(void) {
    setlocale(LC_ALL, "");

#ifdef __windows__
    // Necessário para utilizar as funções de socket no windows
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("%s > WSAStartup falhou: %i.\n", __func__, WSAGetLastError());
        return EXIT_FAILURE;
    }
#endif
    // Definir o endereço e a porta alvo
    // htons (host to network short) e htonl (host to network long)
    // estão sendo usados para converter o valor para ordem big-endian
    struct sockaddr_in addr;
    addr.sin_family = AF_INET; // para endereços ipv4
    addr.sin_port = htons(5000); // porta alvo
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY para endereço local

    // Criar um socket para conexões tcpip
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == SOCKET_ERROR) {
        // __func__ retorna o nome da função atual apenas
        printf("%s > socket falhou: %i.\n", __func__, errno);
        return EXIT_FAILURE;
    }

    // Associar o endereço local e a porta definido ao socket criado
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        printf("%s > bind falhou: %i.\n", __func__, errno);

        close(sockfd);
        return EXIT_FAILURE;
    }

    // Definir o numero de clientes a ser conectados
    listen(sockfd, 2);

    struct sockaddr_in clientaddr;
    int clientaddr_len = sizeof(clientaddr);

    char ipv4_addr[INET_ADDRSTRLEN];
    if (get_ipv4_addr(ipv4_addr, INET_ADDRSTRLEN) == SOCKET_ERROR) {
        wprintf(L"%s > get_ipv4_addr falhou: %i.\n", __func__, WSAGetLastError());

        close(sockfd);
        return EXIT_FAILURE;
    }

    wprintf(L"Endereço: %s\nPorta: %i\n", ipv4_addr, 5000);

    // loop principal
    while (true) {
        wprintf(L"Aguardando conexão.\n");

        // Aguardando até que um cliente seja conectado
        SOCKET client = accept(sockfd, (struct sockaddr*)&clientaddr, &clientaddr_len);
        printf("Conexão recebida.\n");
        break;
    }

    printf("Fechando socket.\n");
    close(sockfd);

#ifdef __windows__
    WSACleanup();
#endif

    printf("Programa finalizado.\n");
    return EXIT_SUCCESS;
}

int get_ipv4_addr(char* buffer, int len) {
    // Recuperando o nome do host local
    char hostname[NI_MAXHOST];
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        wprintf(L"%s > gethostname falhou: %i\n.", __func__, WSAGetLastError());
        return SOCKET_ERROR;
    }

    // hints fornece dicas do que vai ser escrito no ponteiro res pela função getaddrinfo,
    // podemos filtrar apenas endereços ipv4 (AF_INET) por exemplo, especificando
    // na estrutura hints. A estrutura res conterá uma lista encadeada de todos os endereços
    // na forma binaria correspondente ao hostname passado na função getaddrinfo
    struct addrinfo hints, *res;

    // Zerar a estrutura hints
    memset(&hints, 0, sizeof(hints));

    // Definir o que será filtrado
    hints.ai_family = AF_INET;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname, NULL, &hints, &res) != 0) {
        printf("%s > getaddrinfo falhou: %i.\n", __func__, WSAGetLastError());
        return SOCKET_ERROR;
    }

    // Fazer a conversão do endereço de socket (sockaddr) para uma representação
    // de cadeia de caracteres. Será necessário fazer um cast da estrutura
    // sockaddr para sockaddr_in, sockaddr_in contem um campo sin_addr que
    // representa o endereço ip na forma binaria a ser convertida
    struct sockaddr_in* in = (struct sockaddr_in*)res->ai_next->ai_addr;
    inet_ntop(AF_INET, &in->sin_addr, buffer, len);

    if (!buffer) {
        wprintf(L"%s > inet_ntop falhou: %i.\n", __func__, WSAGetLastError());

        freeaddrinfo(res);
        return SOCKET_ERROR;
    }

    freeaddrinfo(res);
    return EXIT_SUCCESS;
}
