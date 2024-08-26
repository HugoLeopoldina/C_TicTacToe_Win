#include "MyPOSIXFunctions.h"
#include <stdbool.h>

int main(void) {
    if (_initialize() == EXIT_FAILURE) {
        wprintf(L"%s > _initialize falhou.\n", __func__);
        return EXIT_FAILURE;
    }

    Server server = {
        .cclient = 2,
        .port = 8000
    };

    if (create_local_server(&server) == EXIT_FAILURE) {
        wprintf(L"%s > create_local_server falhou.\n", __func__);
        return EXIT_FAILURE;
    }

    wprintf( L"\nServidor local criado com sucesso!\n"
              "Nome do Host: %s\n"
              "Endereço: %s\n"
              "Porta: %i\n\n",
               server.hostname,
               server.ipv4_addr,
               server.port );

    SOCKET sock_p1, sock_p2 = INVALID_SOCKET;
    struct sockaddr_in addr_p1, addr_p2;
    int addr_len_p1, addr_len_p2;

    wprintf(L"Aguardando conexões\n");
    while (true) {
        if (sock_p1 == INVALID_SOCKET) {
            sock_p1 = accept(server.sockfd, (struct sockaddr*)&addr_p1, &addr_len_p1);
            wprintf(L"Player 1 conectado\n");
        }

        else if (sock_p2 == INVALID_SOCKET) {
            sock_p2 = accept(server.sockfd, (struct sockaddr*)&addr_p2, &addr_len_p2);
            wprintf(L"Player 2 conectado\n");
        }
    }

    wprintf(L"Servidor fechado.\n");

    freeServer(&server);
    return EXIT_SUCCESS;
}
