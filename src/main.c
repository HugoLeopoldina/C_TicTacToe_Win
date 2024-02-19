#include "utils/functions.h"
#include "utils/structs.h"

int main(void) {
    system("chcp 65001");
    cleanTerminal();

    Game* game = makeGame();

    while (true) {
        printGame(game);
        printTurn(game);
        getChoices(game);

        bool result;

        while ((result = makeChoices(game)) == 1) {
            printError(game, "Posição já definida!");
            getChoices(game);
        }

        void* Result = checkWinner(game);

        if (Result != NULL) {
            if (Result == (void*)1) {
                printTie(game);
            } else {
                printWinner(game, (Player*)Result);
            }
            break;
        }
    }

    endGame(game);
    getchar();
    return EXIT_SUCCESS;
}
