#include "engine/Game.h"
#include "renderer/Render.h"
#include "engine/Builder.h"
#include "engine/World.h"
#include "engine/Entity.h"

static void Game_Update(Game *game);
static void Game_Render(Game *game);
static void Game_Loop(Game *game);

Game * Game_Create(Context *context) {
	Game *game;

	game = Memory_Alloc(context->memory, sizeof(Game));

	game->context = context;

	for(int i = 0; i < WORLD_SIZE; i++) {
		for(int j = 0; j < WORLD_SIZE; j++) {
			Tile tile = {
				.bot_height = 0.0f,
				.top_height = 4.0f,
				.bot_texture = 0,
				.top_texture = 0,
				.wall_texture = 1,
				.bot_window_texture = 1,
				.top_window_texture = 1,
				.wall_type = WALLTYPE_NONE,
			};

			World_EditTile(&game->world, i, j, &tile);
		}
	}

	for(int i = 1; i <= 4; i++){
		Tile tile = {
			.bot_height = (float) i / 8.0f,
			.top_height = 2.0f,
			.bot_texture = 0,
			.top_texture = 0,
			.wall_texture = 1,
			.bot_window_texture = 1,
			.top_window_texture = 1,
			.wall_type = WALLTYPE_NONE,
		};
	
		World_EditTile(&game->world, 5 + i, 5, &tile);
	}

	game->world.collision_layer = 1;

	Builder_BuildMesh(context->stack, &game->world);

	Shader_Load(&game->world.shader,
			Memory_ReadFileAsString(context->memory, "res/shaders/octree.vs"),
			Memory_ReadFileAsString(context->memory, "res/shaders/octree.fs")
			);
	Memory_Free(context->stack);

	TextureArray_Create(&game->world.tile_textures, 64, 64);
	TextureArray_Load(&game->world.tile_textures, "floor.png");
	TextureArray_Load(&game->world.tile_textures, "wall.png");

	Context_SetFps(context, 165);

	Mat4_Transform(&game->view, -4.0f, -0.5f, -4.0f);

	Mat4_PerspectiveProjection(
			&game->projection,
			16.0f / 9.0f,
			3.14 / 4,
			100.0f,
			0.2f
			);

	for(int i = 0; i < MAX_ENTITIES; i++)
		Entity_Reset(&game->entities[i]);

	return game;
}

void Game_Run(Game *game) {
	while(!game->context->quit) {
		Game_Loop(game);
	}
}

Entity * Game_AddEntity(Game *game) {
	Entity *entity;

	for(int i = 0; i < MAX_ENTITIES; i++) {
		entity = &game->entities[i];

		if(entity->unused) {
			Entity_Reset(entity);
			entity->unused = false;
			return entity;
		}
	}

	return NULL;
}

static void Game_Update(Game *game) {
	for(int i = 0; i < MAX_ENTITIES; i++) {
		Entity *entity = &game->entities[i];

		if(!entity->unused) {
			Entity_Update(entity, game, game->context->dt);
		}
	}
}

static void Game_Render(Game *game) {
	Render_Clear(game->context, 0x00, 0x00, 0x00, 0xff);
	World_Render(&game->world, &game->view, &game->projection);
	Render_Present(game->context);
}

static void Game_Loop(Game *game) {
	Context_PollEvent(game->context);

	Game_Update(game);
	Game_Render(game);

	Context_DelayFPS(game->context);
}
