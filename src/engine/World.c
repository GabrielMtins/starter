#include "engine/World.h"
#include <stddef.h>

const Tile * World_GetTile(const World *world, int i, int j) {
	int index;

	if(i < 0 || j < 0 || i >= WORLD_SIZE || j >= WORLD_SIZE)
		return NULL;

	index = i + j * WORLD_SIZE;

	return &world->tiles[index];
}

bool World_EditTile(World *world, int i, int j, const Tile *tile) {
	int index;

	if(i < 0 || j < 0 || i >= WORLD_SIZE || j >= WORLD_SIZE)
		return false;

	index = i + j * WORLD_SIZE;

	world->tiles[index] = *tile;

	return true;
}
