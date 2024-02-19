#include "functions.h"
#include <string.h>

// Faz a limpeza do terminal
void cleanTerminal(void) {
    printf("\033[H\033[J"); // Equivalente ao CTRL + L
}

// Imprime uma menssagem de erro
void printError(Game* game, char* error) {
    printGame(game);
    printf("> \033[31m %s \033[0m\n", error);
    printTurn(game);
}

// Imprime o vencedor
void printWinner(Game* game, Player* winner) {
    cleanTerminal();
    printGame(game);
    printf("%s Ganhou!!\n", winner->name);
}

// Imprime empate
void printTie(Game* game) {
    cleanTerminal();
    printGame(game);
    printf("\nEmpate!!\n");
}

// Cria o objeto principal do jogo e inicializa os membros
Game* makeGame(void) {
    Game* game = (Game*)malloc(sizeof(Game));
    char** table = (char**)malloc(sizeof(char*) * 3);
    Player* players = (Player*)malloc(sizeof(Player) * 2);

    players[0].name = "Player 1";
    players[0].choicesCount = 0;
    players[0].choices = NULL;
    players[0].key = 'X';

    players[1].name = "Player 2";
    players[1].choicesCount = 0;
    players[1].choices = NULL;
    players[1].key = 'O'; 

    for (int i = 0; i < 3; i++) {
        table[i] = malloc(sizeof(char) * 3);

        for (int y = 0; y < 3; y++) {
            table[i][y] = '\0';
        }
    }

    game->table = table;
    game->players = players;
    game->currentPlayer = &game->players[0];

    return game;
}

void endGame(Game* game) {
    for (int row = 0; row < 3; row++) {
        free(game->table[row]);
    }

    for (int i = 0; i < 2; i++) {
        Player* player = &game->players[i];

        for (int c = 0; c < player->choicesCount; c++) {
            free(player->choices[c]);
        }
    }

    free(game->players);
    free(game->table);
    free(game);

    printf("Jogo terminado.\n\n");
}

void printGame(Game* game) {
    cleanTerminal();
    printf("\n   ");
    for (int i = 0; i < 3; i++) {
        printf("%i ", i + 1);
    }

    printf("\n   ");
    for (int i = 0; i < 7; i++) {
        printf(" ");
    }

    printf("\n");
    for (int i = 0; i < 3; i++) {
        printf("%i  ", i + 1);

        for (int y = 0; y < 3; y++) {
            if (game->table[i][y] == 0) {
                printf("  ");
            } else {
                printf("%c ", game->table[i][y]);
            }
        }
        printf("\n");
    }

    printf("\n");
}

void printTurn(Game* game) {
    printf("Turno: %s\n", game->currentPlayer->name);
}

void getChoices(Game* game) {
    int* choices = (int*)malloc(sizeof(int) * 2);

    int* row = &choices[0];
    bool rCondition;

    int* col = &choices[1];
    bool cCondition;

    do {
        printf("Linha: ");
        scanf("%d", &*row);
        while (getchar() != '\n');

        rCondition = (*row - 1) > 2 || (*row - 1) < 0;

        if (rCondition) {
            printError(game, "Limite de linha excedido!");
        }

    } while (rCondition);

    do {
        printf("Coluna: ");
        scanf("%d", &*col);
        while (getchar() != '\n');

        cCondition = (*col - 1) > 2 || (*col - 1) < 0;

        if (cCondition) {
            printError(game, "Limite de coluna excedido!");
            printf("Linha: %d\n", *row);
        }

    } while (cCondition);

    game->currentPlayer->choice = choices;
}

bool makeChoices(Game* game) {
    int* choices = game->currentPlayer->choice;
    char* currentPos = &game->table[choices[0] - 1][choices[1] - 1];

    if (*currentPos == 0) {
        *currentPos = game->currentPlayer->key;
        game->currentPlayer->choicesCount += 1;

    
        game->currentPlayer->choices = (int**)realloc(game->currentPlayer->choices, sizeof(int*) * game->currentPlayer->choicesCount);
        game->currentPlayer->choices[game->currentPlayer->choicesCount - 1] = choices;

        for (int i = 0; i < 2; i++) {
            if (strcmp(game->players[i].name, game->currentPlayer->name) != 0) {
                game->currentPlayer = &game->players[i];
                break;
            }
        }

        return 0;
    } else {
        return 1;
    }
}

void* checkWinner(Game* game) {
    char** table = game->table;
    void* result = NULL;

    for (int i = 0; i < 2; i++) {
        Player* player = &game->players[i];

        int rPoints = 0;
        int cPoints = 0;
        int dPoints = 0;
        int adPoints = 0;

        for (int col = 0; col < 3; col++) {
            for (int row = 0; row < 3; row++) {
                char currentRow = table[col][row];
                char currentCol = table[row][col];
                char currentDiag = table[row][row];
                char currentAntiDiag = table[2-row][row];

                if (currentRow == player->key) {
                    rPoints++;
                } if (currentCol == player->key) {
                    cPoints++;
                } if (currentDiag == player->key) {
                    dPoints++;
                } if (currentAntiDiag == player->key) {
                    adPoints++;
                }
            }

            if (rPoints < 3) {
                rPoints = 0;
            } if (cPoints < 3) {
                cPoints = 0;
            } if (dPoints < 3) {
                dPoints = 0;
            } if (adPoints < 3) {
                adPoints = 0;
            }
        }
        
        if (rPoints >= 3 || cPoints >= 3 || dPoints >= 3 || adPoints >= 3) {
            result = (void*)player;
            break;
        }
    }

    // Verificar empate
    int count = game->players[0].choicesCount + game->players[1].choicesCount;
    if (count == 9) {
        result = (void*)1;
    }

    return result;
}
