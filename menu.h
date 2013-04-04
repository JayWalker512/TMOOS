#ifndef MENU_H
#define MENU_H

#include "games/gamelib.h"

char MENU_Init(void);
char MENU_Loop(ProgData_t *dataTable);
char MENU_CheckEscapeSequence(void);
void MENU_LauncherLoop(void);

#endif