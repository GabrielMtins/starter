#ifndef GAME_H
#define GAME_H

#include "engine/Types.h"

Game * Game_Create(Context *context);

void Game_Run(Game *game);

Entity * Game_AddEntity(Game *game);

#endif
