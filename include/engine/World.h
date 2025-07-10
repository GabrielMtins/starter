#ifndef WORLD_H
#define WORLD_H

#include "engine/Types.h"
#include "base/Mat4.h"

#define World_GetChunkIndex(i, j) ((i / CHUNK_SIZE) + (j / CHUNK_SIZE) * NUM_CHUNKS)
#define Chunk_GetTileIndex(i, j) ((i % CHUNK_SIZE) + (j % CHUNK_SIZE) * CHUNK_SIZE)

const Tile * World_GetTile(const World *world, int i, int j);

bool World_EditTile(World *world, int i, int j, const Tile *tile);

void World_Render(const World *world, const Mat4 *view, const Mat4 *projection);

#endif
