#include "engine/Entity.h"
#include "engine/World.h"

#include <math.h>

#define FLOOR_TOLERANCE (3.0f / 16.0f)

static bool Entity_CheckCollisionWorld(const Entity *entity, const World *world);
static void Entity_HandleFloorTolerance(Entity *entity, const World *world);
static void Entity_HandleUpdatePosition(Entity *entity, const Game *game, float dt);

void Entity_Reset(Entity *entity) {
	entity->position = VEC3_ZERO;
	entity->velocity = VEC3_ZERO;
	entity->size = VEC3_ZERO;

	entity->unused = true;
	entity->health = 100;

	entity->texture = NULL;
	entity->collision_layer = 0;
	entity->collision_mask = 0;
}

void Entity_Update(Entity *entity, Game *game, float dt) {
	if(entity->update != NULL) {
		entity->update(entity, game, dt);
	}

	if(game->context->tick > entity->next_think && entity->think != NULL) {
		entity->think(entity, game);
	}

	if(entity->health < 0) {
		entity->unused = true;
	}

	Entity_HandleUpdatePosition(entity, game, dt);
}

static bool Entity_CheckCollisionWorld(const Entity *entity, const World *world) {
	if(!(entity->collision_mask & world->collision_layer))
		return false;

	return World_CheckCollisionBox(world, &entity->position, &entity->size);
}

static void Entity_HandleFloorTolerance(Entity *entity, const World *world) {
	int min_x, min_z, max_x, max_z;
	const Tile *tile;

	float max_floor_height = -999.0f;

	if(!(entity->collision_mask & world->collision_layer))
		return;

	min_x = floorf(entity->position.x);
	min_z = floorf(entity->position.z);

	max_x = min_x + ceilf(entity->size.x) + 1;
	max_z = min_z + ceilf(entity->size.z) + 1;

	for(int i = min_x; i < max_x; i++) {
		for(int j = min_z; j < max_z; j++) {
			tile = World_GetTile(world, i, j);

			if(tile == NULL)
				continue;

			max_floor_height = max_floor_height > tile->bot_height ? max_floor_height : tile->bot_height;
		}
	}

	if(fabsf(entity->position.y - max_floor_height) < FLOOR_TOLERANCE && max_floor_height > entity->position.y && entity->velocity.y <= 0.0f) {
		entity->position.y = max_floor_height + 0.01f;
	}
}

static void Entity_HandleUpdatePosition(Entity *entity, const Game *game, float dt) {
	Vec3 original;
	const World *world;

	entity->on_floor = false;

	world = &game->world;

	original = entity->position;

	entity->position.x += entity->velocity.x * dt;
	Entity_HandleFloorTolerance(entity, world);

	if(Entity_CheckCollisionWorld(entity, world)) {
		entity->position.x = original.x;
	}

	entity->position.y += entity->velocity.y * dt;

	if(Entity_CheckCollisionWorld(entity, world)) {
		entity->position.y = original.y;

		if(entity->velocity.y < 0.0f) {
			entity->on_floor = true;
		}

		entity->velocity.y = 0.0f;
	}

	entity->position.z += entity->velocity.z * dt;
	Entity_HandleFloorTolerance(entity, world);

	if(Entity_CheckCollisionWorld(entity, world)) {
		entity->position.z = original.z;
	}
}
