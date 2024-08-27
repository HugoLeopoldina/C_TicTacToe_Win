#include "../../lib/MyPOSIXFunctions.h"

int main(void) {
    int result = _initialize();
    if (_initialize() != EXIT_SUCCESS) {
        wprintf(L"%s > _initialize falhou.\n");
        return result;
    }

    

    result = _finish();
    if (result != EXIT_SUCCESS) {
        wprintf(L"%s > _finish falhou.\n");
        return result;
    }

    return EXIT_SUCCESS;
}
