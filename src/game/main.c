#include <stdio.h>

#include "base/Context.h"
#include "base/Memory.h"
#include "renderer/Render.h"
#include "renderer/Texture.h"
#include "renderer/Shader.h"
#include "renderer/Mesh.h"

#include "engine/Builder.h"
#include "engine/Types.h"

#define BASE_MEMORY ( 4 * 1024 * 1024 )

int main(int argc, char **argv) {
	(void) argc;
	(void) argv;

	Memory memory, stack;
	Context *context;
	MegaTexture mega_texture;
	Texture texture;
	Shader shader;
	Mesh mesh;
	Mat4 model;
	World *world;

	memory = Memory_Create(malloc(BASE_MEMORY), BASE_MEMORY);
	stack = Memory_Create(malloc(BASE_MEMORY), BASE_MEMORY);
	context = Context_Create("oi", 1280, 720, &memory, &stack);
	world = Memory_Alloc(&memory, sizeof(world));

	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			world->tiles[i + j * WORLD_SIZE].bot_height = 0.0f;
			world->tiles[i + j * WORLD_SIZE].top_height = 1.0f;
			world->tiles[i + j * WORLD_SIZE].bot_texture = (i + j) % 2;
		}
	}

	world->tiles[2 + 2 * WORLD_SIZE].bot_height = 0.1f;
	world->tiles[2 + 1 * WORLD_SIZE].bot_height = 0.2f;

	Builder_BuildMesh(&mesh, context->stack, world);

	Shader_Load(&shader,
			Memory_ReadFileAsString(&memory, "res/shaders/octree.vs"),
			Memory_ReadFileAsString(&memory, "res/shaders/octree.fs")
			);
	Memory_Free(context->stack);

	MegaTexture_Load(&mega_texture, "floor.png");
	Texture_Create(&texture, &mega_texture, 0, 0, 64, 64, 32, 32);

	while(!context->quit) {
		Context_PollEvent(context);
		Render_Clear(context, 0x00, 0x00, 0x00, 0xff);

		Mat4_Identity(&model);
		Shader_SetUniformMat4(&shader, "model", &model);

		Mat4_Transform(&model, -2.0f, -0.5f, -6.0f);
		Shader_SetUniformMat4(&shader, "view", &model);

		Mat4_PerspectiveProjection(
				&model,
				16.0f / 9.0f,
				3.14 / 4,
				100.0f,
				0.2f
				);

		Shader_SetUniformMat4(&shader, "projection", &model);

		Mesh_Render(&mesh, &shader);

		//Texture_Render(&texture, 0, 0, 0, 0, 160, 120);

		Render_Present(context);
	}

	Context_Destroy(context);
	free(memory.block);

	return 0;
}
