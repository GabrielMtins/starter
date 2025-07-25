#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include "renderer/Mesh.h"
#include "renderer/Texture.h"
#include "base/Context.h"

#define WORLD_SIZE 256
#define CHUNK_SIZE 64
#define NUM_CHUNKS (WORLD_SIZE / CHUNK_SIZE)

#define MAX_TAGS 64
#define MAX_ENTITIES 256

typedef struct Game Game;
typedef struct Entity Entity;

typedef enum {
	WALLTYPE_NONE = 0,
	WALLTYPE_BLOCK,
	WALLTYPE_HALFBLOCK_LEFT,
	WALLTYPE_HALFBLOCK_RIGHT,
	WALLTYPE_HALFBLOCK_UP,
	WALLTYPE_HALFBLOCK_DOWN,

	WALLTYPE_HALFBLOCK_MIDDLE,


	WALLTYPE_DIAGONAL_DOWNLEFT,
	WALLTYPE_DIAGONAL_DOWNRIGHT,
	WALLTYPE_DIAGONAL_UPLEFT,
	WALLTYPE_DIAGONAL_UPRIGHT,

	WALLTYPE_NUMTYPES
} WallType;

typedef struct {
	float bot_height;
	int bot_window_texture;
	int bot_texture;

	float top_height;
	int top_window_texture;
	int top_texture;

	int wall_texture;

	WallType wall_type;
} Tile;

typedef struct { 
	Tile tiles[CHUNK_SIZE * CHUNK_SIZE];
	Mesh mesh;
	bool dirty;
} Chunk;

typedef struct {
	Chunk chunks[NUM_CHUNKS * NUM_CHUNKS];
	Tile tiles[WORLD_SIZE * WORLD_SIZE];
	Mesh chunk_meshes[NUM_CHUNKS * NUM_CHUNKS];
	TextureArray tile_textures;
	Shader shader;

	uint32_t collision_layer;
} World;

struct Entity {
	Vec3 position;
	Vec3 velocity;
	Vec3 direction;
	Vec3 size;

	Vec3 angle;

	int health;

	Entity *target;
	Entity *child;

	Texture *texture;
	uint32_t next_think;

	uint32_t collision_layer;
	uint32_t collision_mask;

	bool unused;
	bool on_floor;

	void (*update)(Entity *, Game *, float dt);
	void (*think)(Entity *, Game *);
};

struct Game {
	Context *context;
	World world;

	Entity entities[MAX_ENTITIES];

	Mat4 view;
	Mat4 projection;

	bool tags[MAX_TAGS];
};

#endif
