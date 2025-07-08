#include <stdio.h>

#include "base/Context.h"
#include "base/Memory.h"
#include "renderer/Render.h"
#include "renderer/Texture.h"
#include "renderer/Shader.h"
#include "renderer/Mesh.h"

#include "engine/World.h"

#include "engine/Builder.h"
#include "engine/Types.h"

#define BASE_MEMORY ( 256 * 1024 * 1024 )

int main(int argc, char **argv) {
	(void) argc;
	(void) argv;

	Memory memory, stack;
	Context *context;
	TextureArray texture_array;
	Texture texture;
	Shader shader;
	Mat4 model;
	World *world;

	memory = Memory_Create(malloc(BASE_MEMORY), BASE_MEMORY);
	stack = Memory_Create(malloc(BASE_MEMORY), BASE_MEMORY);
	context = Context_Create("oi", 1280, 720, &memory, &stack);
	world = Memory_Alloc(&memory, sizeof(world));

	for(int i = 0; i < WORLD_SIZE; i++) {
		for(int j = 0; j < WORLD_SIZE; j++) {
			Tile tile = {
				.bot_height = 0.0f,
				.top_height = 2.0f,
				.bot_texture = 0,
				.top_texture = 0,
				.wall_texture = 1,
				.bot_window_texture = 1,
				.top_window_texture = 1,
				.wall_type = WALLTYPE_NONE,
			};

			World_EditTile(world, i, j, &tile);
			/*
			world->tiles[i + j * WORLD_SIZE].bot_height = 0.0f;
			world->tiles[i + j * WORLD_SIZE].top_height = 1.0f;
			world->tiles[i + j * WORLD_SIZE].bot_texture = (i + j) % 2;
			*/
		}
	}

	world->tiles[0].wall_type = WALLTYPE_HALFBLOCK_DOWN;
	world->tiles[0].bot_height = 0.5f;
	world->tiles[0].top_height = 1.5f;
	world->tiles[2 + 2 * WORLD_SIZE].bot_height = 0.1f;
	world->tiles[2 + 1 * WORLD_SIZE].bot_height = 0.2f;
	world->tiles[2 + 1 * WORLD_SIZE].wall_type = WALLTYPE_HALFBLOCK_MIDDLE;

	world->tiles[2 + 2 * WORLD_SIZE].top_height = 2.8f;
	world->tiles[2 + 1 * WORLD_SIZE].top_height = 2.9f;

	Builder_BuildMesh(context->stack, world);


	Shader_Load(&shader,
			Memory_ReadFileAsString(&memory, "res/shaders/octree.vs"),
			Memory_ReadFileAsString(&memory, "res/shaders/octree.fs")
			);
	Memory_Free(context->stack);

	Texture_Load(&texture, "floor.png", 64, 64);

	TextureArray_Create(&texture_array, 64, 64);
	TextureArray_Load(&texture_array, "floor.png");
	TextureArray_Load(&texture_array, "wall.png");

	Context_SetFps(context, 165);

	while(!context->quit) {
		Context_PollEvent(context);
		Render_Clear(context, 0x00, 0x00, 0x00, 0xff);

		Mat4_Identity(&model);
		Mat4 transform;
		Mat4_Transform(&transform, -2.0, 0.0f, -2.0f);
		Mat4_RotateY(&model, (float) (context->tick) * 0.0005f);
		Mat4_Mul(&model, &model, &transform);
		Shader_SetUniformMat4(&shader, "model", &model);

		Mat4_Transform(&model, -0.0f, -1.0, -3.0f);
		Shader_SetUniformMat4(&shader, "view", &model);

		Mat4_PerspectiveProjection(
				&model,
				16.0f / 9.0f,
				3.14 / 4,
				100.0f,
				0.2f
				);

		Shader_SetUniformMat4(&shader, "projection", &model);

		Mesh_Render(&world->chunk_meshes[0], &shader);

		//Texture_Render(&texture, 0, 0, 0, 0, 160, 120);

		Render_Present(context);

		Context_DelayFPS(context);
		//printf("%f\n", 1.0f / context->dt);
	}

	Context_Destroy(context);
	free(memory.block);

	return 0;
}
