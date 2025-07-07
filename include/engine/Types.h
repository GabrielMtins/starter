#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

#define WORLD_SIZE 256

typedef struct {
	float bot_height;
	int bot_window_texture;
	int bot_texture;

	float top_height;
	int top_window_texture;
	int top_texture;

	bool has_wall;
	int wall_texture;
} Tile;

typedef struct {
	Tile tiles[WORLD_SIZE * WORLD_SIZE];
} World;

#endif
