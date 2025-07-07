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

static void Builder_AllocVertices(BuilderContext *context, size_t num_vertices);
static void Builder_AllocIndices(BuilderContext *context, size_t num_indices);

static void Builder_BuildTileX(BuilderContext *context, const World *world, int i, int j);
static void Builder_BuildTileY(BuilderContext *context, const World *world, int i, int j);
static void Builder_BuildTileZ(BuilderContext *context, const World *world, int i, int j);
static void Builder_BuildTileWall(BuilderContext *context, const World *world, int i, int j);

static void Builder_BuildTile(BuilderContext *context, const World *world, int i, int j);

static void Builder_BuildPlaneX(BuilderContext *context, const Vec3 *position, float height, float texture);
static void Builder_BuildPlaneY(BuilderContext *context, const Vec3 *position, float texture);
static void Builder_BuildPlaneZ(BuilderContext *context, const Vec3 *position, float height, float texture);

bool Builder_BuildMesh(Mesh *mesh, Memory *stack, const World *world) {
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

	for(int i = 0; i < WORLD_SIZE; i++) {
		for(int j = 0; j < WORLD_SIZE; j++) {
			Builder_BuildTile(&context, world, i, j);
		}
	}

	Mesh_Create(
			mesh,
			context.vertices,
			context.vcount,
			context.indices,
			context.icount
			);

	printf("%lu\n", used / 1024 / 1024);
	printf("%lu\n", context.vcount * sizeof(Vertex) / 1024 / 1024);

	Memory_RestoreState(stack, old_state);

	return true;
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

	if(tile->has_wall || tile->bot_height == tile->top_height)
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

static void Builder_BuildTileWall(BuilderContext *context, const World *world, int i, int j) {
	const Tile *tile = World_GetTile(world, i, j);
	float wall_diff;
	Vec3 position;

	if(!tile->has_wall)
		return;

	wall_diff = tile->top_height - tile->bot_height;

	position = (Vec3) {i, tile->bot_height, j};
	Builder_BuildPlaneX(context, &position, wall_diff, tile->wall_texture);
	Builder_BuildPlaneZ(context, &position, wall_diff, tile->wall_texture);
	
	position = (Vec3) {i + 1, tile->bot_height, j};
	Builder_BuildPlaneX(context, &position, wall_diff, tile->wall_texture);
	
	position = (Vec3) {i, tile->bot_height, j + 1};
	Builder_BuildPlaneZ(context, &position, wall_diff, tile->wall_texture);
}

static void Builder_BuildTile(BuilderContext *context, const World *world, int i, int j) {
	Builder_BuildTileX(context, world, i, j);
	Builder_BuildTileY(context, world, i, j);
	Builder_BuildTileZ(context, world, i, j);
	Builder_BuildTileWall(context, world, i, j);
}

static void Builder_BuildPlaneX(BuilderContext *context, const Vec3 *position, float height, float texture) {
	Builder_AllocVertices(context, 4);

	for(int i = 0; i < 4; i++) {
		Vertex *vertex = &context->vertices[context->vcount - i - 1];
		float add_x = (i & 1) ? 1.0f : 0.0f;
		float add_y = (i & 2) ? height : 0.0f;

		Vertex_CreateSimple(
				vertex,
				position->x,
				position->y + add_y,
				position->z + add_x,
				add_x,
				add_y
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

static void Builder_BuildPlaneY(BuilderContext *context, const Vec3 *position, float texture) {
	Builder_AllocVertices(context, 4);

	for(int i = 0; i < 4; i++) {
		Vertex *vertex = &context->vertices[context->vcount - i - 1];
		float add_x = (i & 1) ? 1.0f : 0.0f;
		float add_y = (i & 2) ? 1.0f : 0.0f;

		Vertex_CreateSimple(
				vertex,
				position->x + add_x,
				position->y,
				position->z + add_y,
				add_x,
				add_y
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

static void Builder_BuildPlaneZ(BuilderContext *context, const Vec3 *position, float height, float texture) {
	Builder_AllocVertices(context, 4);

	for(int i = 0; i < 4; i++) {
		Vertex *vertex = &context->vertices[context->vcount - i - 1];
		float add_x = (i & 1) ? 1.0f : 0.0f;
		float add_y = (i & 2) ? height : 0.0f;

		Vertex_CreateSimple(
				vertex,
				position->x + add_x,
				position->y + add_y,
				position->z,
				add_x,
				add_y
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
