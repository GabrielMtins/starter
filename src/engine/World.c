#include "engine/World.h"
#include <stddef.h>

#include <math.h>

#include "base/Box.h"

#define MIN_HEIGHT -999.0f
#define MAX_HEIGHT 999.0f

static bool World_CheckCollisionFloor(const World *world, int i, int j, const Vec3 *position, const Vec3 *size);
static bool World_CheckCollisionCeiling(const World *world, int i, int j, const Vec3 *position, const Vec3 *size);
static bool World_CheckCollisionWall(const World *world, int i, int j, const Vec3 *position, const Vec3 *size);

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

bool World_CheckCollisionBox(const World *world, const Vec3 *position, const Vec3 *size) {
	int min_x = (int) floorf(position->x);
	int min_z = (int) floorf(position->z);

	int max_x = min_x + (int) ceilf(size->x);
	int max_z = min_z + (int) ceilf(size->z);

	for(int i = min_x; i <= max_x; i++) {
		for(int j = min_z; j <= max_z; j++) {
			if(World_CheckCollisionFloor(world, i, j, position, size))
				return true;

			if(World_CheckCollisionCeiling(world, i, j, position, size))
				return true;

			if(World_CheckCollisionWall(world, i, j, position, size))
				return true;
		}
	}
	
	return false;
}

static bool World_CheckCollisionFloor(const World *world, int i, int j, const Vec3 *position, const Vec3 *size) {
	const Tile *tile;
	Vec3 tile_position, tile_size;

	tile = World_GetTile(world, i, j);

	if(tile == NULL)
		return true;

	tile_size = (Vec3) {1.0f, tile->bot_height - MIN_HEIGHT, 1.0f};
	tile_position = (Vec3) {i, 0.0f + MIN_HEIGHT, j};

	return Box_CheckCollision(position, size, &tile_position, &tile_size);
}

static bool World_CheckCollisionCeiling(const World *world, int i, int j, const Vec3 *position, const Vec3 *size) {
	const Tile *tile;
	Vec3 tile_position, tile_size;

	tile = World_GetTile(world, i, j);

	if(tile == NULL)
		return true;

	tile_size = (Vec3) {1.0f, MAX_HEIGHT - tile->top_height, 1.0f};
	tile_position = (Vec3) {i, tile->top_height, j};

	return Box_CheckCollision(position, size, &tile_position, &tile_size);
}

static bool World_CheckCollisionWall(const World *world, int i, int j, const Vec3 *position, const Vec3 *size) {
	const Tile *tile;
	Vec3 tile_position, tile_size, normal;

	tile = World_GetTile(world, i, j);

	if(tile == NULL)
		return true;

	tile_size = (Vec3) {1.0f, tile->top_height - tile->bot_height, 1.0f};
	tile_position = (Vec3) {i, tile->bot_height, j};

	switch(tile->wall_type) {
		case WALLTYPE_NONE:
			return false;
			break;

		case WALLTYPE_BLOCK:
			return Box_CheckCollision(position, size, &tile_position, &tile_size);
			break;

		case WALLTYPE_HALFBLOCK_DOWN:
			tile_size.z = 0.5f;
			return Box_CheckCollision(position, size, &tile_position, &tile_size);
			break;

		case WALLTYPE_HALFBLOCK_UP:
			tile_position.z += 0.5f;
			tile_size.z = 0.5f;
			return Box_CheckCollision(position, size, &tile_position, &tile_size);
			break;

		case WALLTYPE_HALFBLOCK_LEFT:
			tile_size.x = 0.5f;
			return Box_CheckCollision(position, size, &tile_position, &tile_size);
			break;

		case WALLTYPE_HALFBLOCK_RIGHT:
			tile_position.x += 0.5f;
			tile_size.x = 0.5f;
			return Box_CheckCollision(position, size, &tile_position, &tile_size);
			break;

		case WALLTYPE_HALFBLOCK_MIDDLE:
			tile_position.x += 0.25f;
			tile_position.z += 0.25f;
			tile_size.x = 0.5f;
			tile_size.z = 0.5f;
			return Box_CheckCollision(position, size, &tile_position, &tile_size);
			break;

		case WALLTYPE_DIAGONAL_DOWNLEFT:
			tile_position.x += 1.0f;
			normal = (Vec3) {1.0f, 0.0f, 1.0f};
			return Box_CheckCollisionSemiSpace(position, size, &tile_position, &normal);
			break;

		case WALLTYPE_DIAGONAL_DOWNRIGHT:
			normal = (Vec3) {-1.0f, 0.0f, 1.0f};
			return Box_CheckCollisionSemiSpace(position, size, &tile_position, &normal);
			break;

		case WALLTYPE_DIAGONAL_UPLEFT:
			normal = (Vec3) {1.0f, 0.0f, -1.0f};
			return Box_CheckCollisionSemiSpace(position, size, &tile_position, &normal);
			break;

		case WALLTYPE_DIAGONAL_UPRIGHT:
			tile_position.x += 1.0f;
			normal = (Vec3) {-1.0f, 0.0f, -1.0f};
			return Box_CheckCollisionSemiSpace(position, size, &tile_position, &normal);
			break;

		default:
			return true;
			break;
	}

	return false;
}
