#include "engine/Builder.h" 

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
static void Builder_BuildTile(BuilderContext *context, const World *world, int i, int j);
static void Builder_BuildZPlane(BuilderContext *context, const Vec3 *position, float texture);

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

	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
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

static void Builder_BuildTile(BuilderContext *context, const World *world, int i, int j) {
	const Tile *tile = &world->tiles[i + j * WORLD_SIZE];
	Vec3 position = {i, tile->bot_height, j};

	Builder_BuildZPlane(context, &position, tile->bot_texture);
	position.y = tile->top_height;
	Builder_BuildZPlane(context, &position, tile->bot_texture);
}

static void Builder_BuildZPlane(BuilderContext *context, const Vec3 *position, float texture) {
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
