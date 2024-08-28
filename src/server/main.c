#include "MyPOSIXFunctions.h"
#include <stdbool.h>

int main(void) {
    int result = _initialize();
    if (_initialize() != EXIT_SUCCESS) {
        wprintf(L"%s > _initialize falhou.\n");
        return result;
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

    SOCKET player_1 = INVALID_SOCKET;
    SOCKET player_2 = INVALID_SOCKET;

    struct sockaddr_in addr_p1 = { 0 };
    struct sockaddr_in addr_p2 = { 0 };

    int sock_len = sizeof(struct sockaddr_in);

    wprintf(L"Aguardando conexões\n");
    while (true) {
        if (player_1 == INVALID_SOCKET) {
            player_1 = accept(server.sockfd, (struct sockaddr*)&addr_p1, &sock_len);
            wprintf(L"Player 1 conectado\n");
        }

        else if (player_2 == INVALID_SOCKET) {
            player_2 = accept(server.sockfd, (struct sockaddr*)&addr_p2, &sock_len);
            wprintf(L"Player 2 conectado\n");
        }
    }

    wprintf(L"Servidor fechado.\n");

    freeServer(&server);

    result = _finish();
    if (result != EXIT_SUCCESS) {
        wprintf(L"%s > _finish falhou.\n");
        return result;
    }

    return result;
}
