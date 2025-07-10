#include "engine/Game.h"
#include "renderer/Render.h"

static void Game_Loop(Game *game);

Game * Game_Create(Context *context) {
	Game *game;
	Scene *scene;

	game = Memory_Alloc(context->memory, sizeof(Game));

	game->context = context;

	return game;
}

void Game_Run(Game *game) {
	while(game->context->quit) {
		Game_Loop(game);
	}
}

static void Game_Loop(Game *game) {
	Context_PollEvent(game->context);
	Render_Clear(game->context, 0x00, 0x00, 0x00, 0xff);

	Render_Present(game->context);
	Context_DelayFPS(game->context);
}
