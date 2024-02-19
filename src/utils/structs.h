#ifndef _STRUCTS
#define _STRUCTS

/*
    Estrutura Player:

    -> Uma estrutura que conterá informações sobre o player,
       poderá ser tanto um player quanto a cpu

    - choicesCount: usado para contar a quantidade de escolhas
    - choice: armazena a escolha atual do Player
    - choices: aramazena todas as escolhas em uma lista
    - name: nome do Player
    - key: um caractere que será usado para identificar o Player na tabela
*/

typedef struct {
    int choicesCount;
    int** choices;
    int* choice;
    char* name;
    char key;
} Player;

typedef struct {
    Player* currentPlayer;
    Player* players;
    char** table;
} Game;

#endif
