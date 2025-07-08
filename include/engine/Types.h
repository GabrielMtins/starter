#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include "renderer/Mesh.h"

#define WORLD_SIZE 256
#define CHUNK_SIZE 64
#define NUM_CHUNKS (WORLD_SIZE / CHUNK_SIZE)

typedef enum {
	WALLTYPE_BLOCK = 0,
	WALLTYPE_HALFBLOCK_LEFT,
	WALLTYPE_HALFBLOCK_RIGHT,
	WALLTYPE_HALFBLOCK_UP,
	WALLTYPE_HALFBLOCK_DOWN,
} WallType;

typedef struct {
	float bot_height;
	int bot_window_texture;
	int bot_texture;

	float top_height;
	int top_window_texture;
	int top_texture;

	bool has_wall;
	int wall_texture;

	WallType wall_type;
} Tile;

typedef struct {
	Tile tiles[WORLD_SIZE * WORLD_SIZE];
	Mesh chunk_meshes[NUM_CHUNKS * NUM_CHUNKS];
} World;

#endif
