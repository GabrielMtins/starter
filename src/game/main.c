#include <stdio.h>

#include "base/Context.h"
#include "renderer/Render.h"
#include "renderer/Texture.h"

#define BASE_MEMORY ( 4 * 1024 * 1024 )

int main(int argc, char **argv) {
	(void) argc;
	(void) argv;

	Memory memory;
	Context *context;
	MegaTexture mega_texture;
	Texture texture;

	memory = Memory_Create(malloc(BASE_MEMORY), BASE_MEMORY);
	context = Context_Create("oi", 640, 480, &memory, NULL);

	printf("Hello World!\n");

	MegaTexture_Load(&mega_texture, "floor.png");
	Texture_Create(&texture, &mega_texture, 0, 0, 64, 64, 32, 32);

	while(!context->quit) {
		Context_PollEvent(context);
		Render_Clear(context, 0xff, 0xff, 0xff, 0xff);

		Texture_Render(&texture, 0, 0, 0, 0, 160, 120);

		Render_Present(context);
	}

	Context_Destroy(context);
	free(memory.block);

	return 0;
}
