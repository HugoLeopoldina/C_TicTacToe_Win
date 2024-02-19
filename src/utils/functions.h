#ifndef _FUNCTIONS
#define _FUNCTIONS

#include <stdio.h>
#include <stdlib.h>

#include "defines.h"
#include "structs.h"

Game* makeGame(void);
void endGame(Game*);

void printError(Game*, char*);
void printTurn(Game*);
void printGame(Game*);
void printWinner(Game*, Player*);
void printTie(Game*);

void getChoices(Game*);
bool makeChoices(Game*);
void* checkWinner(Game*);

void cleanTerminal(void);

#endif
