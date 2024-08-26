#include "MyPOSIXFunctions.h"

#pragma comment(lib, "MyPOSIXFunctions.lib")

int main(void) {
    SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    char *server_addr = get_ipv4_addr(NULL);
    if (!server_addr) {
        wprintf(L"%s > get_ipv4_addr falhou.\n");
        return EXIT_FAILURE;
    }

    myfree(&server_addr);
    return EXIT_SUCCESS;
}
