#include "engine/Builder.h" 
#include "engine/World.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct {
	Vertex *vertices;
	size_t vcount;
	size_t max_vertices;
	unsigned int *indices;
	size_t icount;
	size_t max_indices;
} BuilderContext;

static void Builder_BuildChunk(Mesh *mesh, Memory *stack, const World *world, int x, int y);
static void Builder_AllocVertices(BuilderContext *context, size_t num_vertices);
static void Builder_AllocIndices(BuilderContext *context, size_t num_indices);

static void Builder_BuildTileX(BuilderContext *context, const World *world, int i, int j);
static void Builder_BuildTileY(BuilderContext *context, const World *world, int i, int j);
static void Builder_BuildTileZ(BuilderContext *context, const World *world, int i, int j);

static void Builder_BuildTileWallGenericBlock(BuilderContext *context, const World *world, int i, int j, const Vec3 *offset, const Vec3 *size);
static void Builder_BuildTileWallBlock(BuilderContext *context, const World *world, int i, int j);
static void Builder_BuildTileWall(BuilderContext *context, const World *world, int i, int j);

static void Builder_BuildTile(BuilderContext *context, const World *world, int i, int j);

static void Builder_BuildPlane(BuilderContext *context, const Vec3 *position, const Vec3 *add, float texture);
static void Builder_BuildPlaneX(BuilderContext *context, const Vec3 *position, float height, float texture);
static void Builder_BuildPlaneY(BuilderContext *context, const Vec3 *position, float texture);
static void Builder_BuildPlaneZ(BuilderContext *context, const Vec3 *position, float height, float texture);

void Builder_BuildMesh(Memory *stack, World *world) {
	for(int i = 0; i < NUM_CHUNKS; i++) {
		for(int j = 0; j < NUM_CHUNKS; j++) {
			int index = i + j * NUM_CHUNKS;

			Builder_BuildChunk(&world->chunk_meshes[index], stack, world, i, j);
		}
	}
}

static void Builder_BuildChunk(Mesh *mesh, Memory *stack, const World *world, int x, int y) {
	size_t used = stack->size / 4;
	size_t old_state = Memory_SaveState(stack);

	BuilderContext context = {
		Memory_Alloc(stack, used),
		0,
		used / sizeof(Vertex),
		Memory_Alloc(stack, used),
		0,
		used / sizeof(unsigned int)
	};

	for(int i = 0; i < CHUNK_SIZE; i++) {
		for(int j = 0; j < CHUNK_SIZE; j++) {
			Builder_BuildTile(
					&context,
					world,
					x * CHUNK_SIZE + i,
					y * CHUNK_SIZE + j
					);
		}
	}

	Mesh_Create(
			mesh,
			context.vertices,
			context.vcount,
			context.indices,
			context.icount
			);

	Memory_RestoreState(stack, old_state);
}

static void Builder_AllocVertices(BuilderContext *context, size_t num_vertices) {
	if(context->vcount + num_vertices > context->max_vertices) {
		fprintf(stderr, "Not enough memory for loading vertex data.");
		exit(1);
	}

	context->vcount += num_vertices;
}

static void Builder_AllocIndices(BuilderContext *context, size_t num_indices) {
	if(context->icount + num_indices > context->max_indices) {
		fprintf(stderr, "Not enough memory for loading index data.");
		exit(1);
	}

	context->icount += num_indices;
}

static void Builder_BuildTileX(BuilderContext *context, const World *world, int i, int j) {
	const Tile *tile = World_GetTile(world, i, j);
	const Tile *left = World_GetTile(world, i - 1, j);
	const Tile *right = World_GetTile(world, i + 1, j);
	Vec3 position = {i, tile->bot_height, j};

	float bot_diff = 0.0f, top_diff = 0.0f;

	if(left != NULL) {
		bot_diff = left->bot_height - tile->bot_height;
		top_diff = left->top_height - tile->top_height;
	}

	if(bot_diff < 0.0f) {
		Builder_BuildPlaneX(context, &position, bot_diff, tile->bot_window_texture);
	}

	if(top_diff > 0.0f) {
		position.y = tile->top_height;
		Builder_BuildPlaneX(context, &position, top_diff, tile->top_window_texture);
	}

	position = (Vec3) {i + 1, tile->bot_height, j};

	if(right != NULL) {
		bot_diff = right->bot_height - tile->bot_height;
		top_diff = right->top_height - tile->top_height;
	}

	if(bot_diff < 0.0f) {
		Builder_BuildPlaneX(context, &position, bot_diff, tile->bot_window_texture);
	}

	if(top_diff > 0.0f) {
		position.y = tile->top_height;
		Builder_BuildPlaneX(context, &position, top_diff, tile->top_window_texture);
	}
}

static void Builder_BuildTileY(BuilderContext *context, const World *world, int i, int j) {
	const Tile *tile = World_GetTile(world, i, j);
	Vec3 position = {i, tile->bot_height, j};

	if(tile->bot_height == tile->top_height)
		return;

	Builder_BuildPlaneY(context, &position, tile->bot_texture);
	position.y = tile->top_height;
	Builder_BuildPlaneY(context, &position, tile->top_texture);
}

static void Builder_BuildTileZ(BuilderContext *context, const World *world, int i, int j) {
	const Tile *tile = World_GetTile(world, i, j);
	const Tile *left = World_GetTile(world, i, j - 1);
	const Tile *right = World_GetTile(world, i, j + 1);
	Vec3 position = {i, tile->bot_height, j};

	float bot_diff = 0.0f, top_diff = 0.0f;

	if(left != NULL) {
		bot_diff = left->bot_height - tile->bot_height;
		top_diff = left->top_height - tile->top_height;
	}

	if(bot_diff < 0.0f) {
		Builder_BuildPlaneZ(context, &position, bot_diff, tile->bot_window_texture);
	}

	if(top_diff > 0.0f) {
		position.y = tile->top_height;
		Builder_BuildPlaneZ(context, &position, top_diff, tile->top_window_texture);
	}

	position = (Vec3) {i, tile->bot_height, j + 1};

	if(right != NULL) {
		bot_diff = right->bot_height - tile->bot_height;
		top_diff = right->top_height - tile->top_height;
	}

	if(bot_diff < 0.0f) {
		Builder_BuildPlaneZ(context, &position, bot_diff, tile->bot_window_texture);
	}

	if(top_diff > 0.0f) {
		position.y = tile->top_height;
		Builder_BuildPlaneZ(context, &position, top_diff, tile->top_window_texture);
	}
}

static void Builder_BuildTileWallGenericBlock(BuilderContext *context, const World *world, int i, int j, const Vec3 *offset, const Vec3 *size) {
	const Tile *tile = World_GetTile(world, i, j);
	float wall_diff;
	Vec3 position, add;
	float start_x, start_z;

	start_x = (float) i + offset->x;
	start_z = (float) j + offset->z;

	wall_diff = tile->top_height - tile->bot_height;

	position = (Vec3) {start_x, tile->bot_height, start_z};
	add = (Vec3) {0.0f, wall_diff, size->z};
	Builder_BuildPlane(context, &position, &add, tile->wall_texture);

	add = (Vec3) {size->x, wall_diff, 0.0f};
	Builder_BuildPlane(context, &position, &add, tile->wall_texture);

	position = (Vec3) {start_x + size->x, tile->bot_height, start_z};
	add = (Vec3) {0.0f, wall_diff, size->z};
	Builder_BuildPlane(context, &position, &add, tile->wall_texture);

	position = (Vec3) {start_x, tile->bot_height, start_z + size->z};
	add = (Vec3) {size->x, wall_diff, 0.0f};
	Builder_BuildPlane(context, &position, &add, tile->wall_texture);
}

static void Builder_BuildTileWallBlock(BuilderContext *context, const World *world, int i, int j) {
	const Tile *tile = World_GetTile(world, i, j);
	Vec3 offset, size;

	if(!tile->has_wall)
		return;

	switch(tile->wall_type) {
		case WALLTYPE_BLOCK:
			offset = (Vec3) {0.0f, 0.0f, 0.0f};
			size = (Vec3) {1.0f, 0.0f, 1.0f};
			break;

		case WALLTYPE_HALFBLOCK_DOWN:
			offset = (Vec3) {0.0f, 0.0f, 0.0f};
			size = (Vec3) {1.0f, 0.0f, 0.5f};
			break;

		case WALLTYPE_HALFBLOCK_UP:
			offset = (Vec3) {0.0f, 0.0f, 0.5f};
			size = (Vec3) {1.0f, 0.0f, 0.5f};
			break;

		case WALLTYPE_HALFBLOCK_LEFT:
			offset = (Vec3) {0.0f, 0.0f, 0.0f};
			size = (Vec3) {0.5f, 0.0f, 1.0f};
			break;

		case WALLTYPE_HALFBLOCK_RIGHT:
			offset = (Vec3) {0.5f, 0.0f, 0.0f};
			size = (Vec3) {0.5f, 0.0f, 1.0f};
			break;
	}

	Builder_BuildTileWallGenericBlock(context, world, i, j, &offset, &size);
}

static void Builder_BuildTileWall(BuilderContext *context, const World *world, int i, int j) {
	Builder_BuildTileWallBlock(context, world, i, j);
}

static void Builder_BuildTile(BuilderContext *context, const World *world, int i, int j) {
	Builder_BuildTileX(context, world, i, j);
	Builder_BuildTileY(context, world, i, j);
	Builder_BuildTileZ(context, world, i, j);
	Builder_BuildTileWall(context, world, i, j);
}

static void Builder_BuildPlane(BuilderContext *context, const Vec3 *position, const Vec3 *add, float texture) {
	Vertex *vertex;
	float add_x, add_y, add_z, u, v;

	Builder_AllocVertices(context, 4);

	for(int i = 0; i < 4; i++) {
		vertex = &context->vertices[context->vcount - i - 1];

		if(add->x == 0.0f) {
			add_x = 0;
			add_y = (i & 1) ? add->y : 0.0f;
			add_z = (i & 2) ? add->z : 0.0f;
			u = add_z + position->z;
			v = add_y + position->y;
		}
		else if(add->y == 0.0f) {
			add_x = (i & 1) ? add->x : 0.0f;
			add_y = 0;
			add_z = (i & 2) ? add->z : 0.0f;
			u = add_x + position->x;
			v = add_z + position->z;
		}
		else {
			add_x = (i & 1) ? add->x : 0.0f;
			add_y = (i & 2) ? add->y : 0.0f;
			add_z = 0;
			u = add_x + position->x;
			v = add_y + position->y;
		}

		Vertex_CreateSimple(
				vertex,
				position->x + add_x,
				position->y + add_y,
				position->z + add_z,
				u,
				v
				);

		vertex->layer_index = texture;
	}

	Builder_AllocIndices(context, 6);

	context->indices[context->icount - 1] = context->vcount - 1;
	context->indices[context->icount - 2] = context->vcount - 2;
	context->indices[context->icount - 3] = context->vcount - 3;

	context->indices[context->icount - 4] = context->vcount - 4;
	context->indices[context->icount - 5] = context->vcount - 2;
	context->indices[context->icount - 6] = context->vcount - 3;
}

static void Builder_BuildPlaneX(BuilderContext *context, const Vec3 *position, float height, float texture) {
	Vec3 add = { 0.0f, height, 1.0f };
	Builder_BuildPlane(context, position, &add, texture);
}

static void Builder_BuildPlaneY(BuilderContext *context, const Vec3 *position, float texture) {
	Vec3 add = { 1.0f, 0.0f, 1.0f };
	Builder_BuildPlane(context, position, &add, texture);
}

static void Builder_BuildPlaneZ(BuilderContext *context, const Vec3 *position, float height, float texture) {
	Vec3 add = { 1.0f, height, 0.0f };
	Builder_BuildPlane(context, position, &add, texture);
}
