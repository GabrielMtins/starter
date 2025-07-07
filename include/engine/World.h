#ifndef WORLD_H
#define WORLD_H

#include "engine/Types.h"

const Tile * World_GetTile(const World *world, int i, int j);

bool World_EditTile(World *world, int i, int j, const Tile *tile);

#endif
