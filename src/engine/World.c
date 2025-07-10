#include "engine/World.h"
#include <stddef.h>

const Tile * World_GetTile(const World *world, int i, int j) {
	const Chunk *chunk;

	if(i < 0 || j < 0 || i >= WORLD_SIZE || j >= WORLD_SIZE)
		return NULL;

	chunk = &world->chunks[World_GetChunkIndex(i, j)];

	return &chunk->tiles[Chunk_GetTileIndex(i, j)];
}

bool World_EditTile(World *world, int i, int j, const Tile *tile) {
	Chunk *chunk;

	if(i < 0 || j < 0 || i >= WORLD_SIZE || j >= WORLD_SIZE)
		return false;

	chunk = &world->chunks[World_GetChunkIndex(i, j)];

	chunk->tiles[Chunk_GetTileIndex(i, j)] = *tile;

	return true;
}

void World_Render(const World *world, const Mat4 *view, const Mat4 *projection) {
	Mat4 model;

	Mat4_Identity(&model);
	Shader_SetUniformMat4(&world->shader, "model", &model);
	Shader_SetUniformMat4(&world->shader, "view", view);
	Shader_SetUniformMat4(&world->shader, "projection", projection);

	TextureArray_Use(&world->tile_textures, 0);
	Shader_SetUniform1i(&world->shader, "tex_array", 0);
	
	for(int i = 0; i < NUM_CHUNKS * NUM_CHUNKS; i++) {
		Mesh_Render(
				&world->chunks[i].mesh,
				&world->shader
				);
	}
}
