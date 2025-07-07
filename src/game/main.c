#include <stdio.h>

#include "base/Context.h"
#include "base/Memory.h"
#include "renderer/Render.h"
#include "renderer/Texture.h"
#include "renderer/Shader.h"
#include "renderer/Mesh.h"

#define BASE_MEMORY ( 4 * 1024 * 1024 )

int main(int argc, char **argv) {
	(void) argc;
	(void) argv;

	Memory memory, stack;
	Context *context;
	MegaTexture mega_texture;
	Texture texture;
	Shader shader;
	size_t old_state;
	Mesh mesh;
	Mat4 model;

	memory = Memory_Create(malloc(BASE_MEMORY), BASE_MEMORY);
	stack = Memory_Create(malloc(BASE_MEMORY), BASE_MEMORY);
	context = Context_Create("oi", 640, 480, &memory, &stack);

	old_state = Memory_SaveState(context->stack);
	Mesh_BuildUnitTetrahedron(&mesh);

	Shader_Load(&shader,
			Memory_ReadFileAsString(&memory, "res/shaders/octree.vs"),
			Memory_ReadFileAsString(&memory, "res/shaders/octree.fs")
			);

	Memory_RestoreState(context->stack, old_state);

	MegaTexture_Load(&mega_texture, "floor.png");
	Texture_Create(&texture, &mega_texture, 0, 0, 64, 64, 32, 32);

	while(!context->quit) {
		Context_PollEvent(context);
		Render_Clear(context, 0x00, 0x00, 0x00, 0xff);

		Mat4_Identity(&model);
		Mat4_RotateX(&model, 3.0f);
		Shader_SetUniformMat4(&shader, "model", &model);

		Mat4_Transform(&model, 0.0f, 0.0f, -2.0f);
		Shader_SetUniformMat4(&shader, "view", &model);

		Mat4_PerspectiveProjection(
				&model,
				1.33,
				3.14 / 3,
				10.0f,
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
