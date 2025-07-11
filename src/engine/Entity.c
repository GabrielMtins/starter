#include "engine/Entity.h"
#include "engine/World.h"

void Entity_Reset(Entity *entity) {
	entity->position = VEC3_ZERO;
	entity->velocity = VEC3_ZERO;
	entity->size = VEC3_ZERO;

	entity->unused = true;

	entity->texture = NULL;
}

void Entity_Update(Entity *entity, Game *game, float dt) {
	Vec3 delta;
	Vec3 original;

	(void) (game);
	(void) (entity);
	(void) (dt);

	original = entity->position;

	entity->position.x += entity->velocity.x * dt;

	if(World_CheckCollisionBox(&game->world, &entity->position, &entity->size)) {
		entity->position.x = original.x;
	}

	entity->position.y += entity->velocity.y * dt;

	if(World_CheckCollisionBox(&game->world, &entity->position, &entity->size)) {
		entity->position.y = original.y;
	}

	entity->position.z += entity->velocity.z * dt;

	if(World_CheckCollisionBox(&game->world, &entity->position, &entity->size)) {
		entity->position.z = original.z;
	}

	if(entity->update != NULL) {
		entity->update(entity, game, dt);
	}

	if(game->context->tick > entity->next_think && entity->think != NULL) {
		entity->think(entity, game);
	}
}
