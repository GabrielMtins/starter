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
	Mat4 view, projection;
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
		}
	}

	Builder_BuildMesh(context->stack, world);

	Shader_Load(&world->shader,
			Memory_ReadFileAsString(&memory, "res/shaders/octree.vs"),
			Memory_ReadFileAsString(&memory, "res/shaders/octree.fs")
			);
	Memory_Free(context->stack);

	TextureArray_Create(&world->tile_textures, 64, 64);
	TextureArray_Load(&world->tile_textures, "floor.png");
	TextureArray_Load(&world->tile_textures, "wall.png");

	Context_SetFps(context, 165);

	while(!context->quit) {
		Mat4 rotation;
		Context_PollEvent(context);
		Render_Clear(context, 0x00, 0x00, 0x00, 0xff);

		Mat4_Transform(&view, -4.0f, -0.5f, -4.0f);
		Mat4_RotateY(&rotation, (float) (context->tick) * 0.0005f * (-1.0f));
		Mat4_Mul(&view, &rotation, &view);

		Mat4_PerspectiveProjection(
				&projection,
				16.0f / 9.0f,
				3.14 / 4,
				100.0f,
				0.2f
				);

		World_Render(world, &view, &projection);

		//Texture_Render(&texture, 0, 0, 0, 0, 160, 120);

		Render_Present(context);

		Context_DelayFPS(context);
		//printf("%f\n", 1.0f / context->dt);
	}

	Context_Destroy(context);
	free(memory.block);

	return 0;
}
