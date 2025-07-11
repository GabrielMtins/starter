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

#include "engine/Game.h"

#include "game/Player.h"

#define BASE_MEMORY ( 16 * 1024 * 1024 )
#define STACK_MEMORY ( 8 * 1024 * 1024 )

int main(int argc, char **argv) {
	(void) argc;
	(void) argv;

	Memory memory, stack;
	Context *context;
	Game *game;

	memory = Memory_Create(malloc(BASE_MEMORY), BASE_MEMORY);
	stack = Memory_Create(malloc(STACK_MEMORY), STACK_MEMORY);

	context = Context_Create("oi", 1280, 720, &memory, &stack);
	game = Game_Create(context);

	Player_Create(Game_AddEntity(game));

	Game_Run(game);

	printf("%lu KB\n", memory.top / 1024 );

	Context_Destroy(context);
	free(memory.block);

	return 0;
}
