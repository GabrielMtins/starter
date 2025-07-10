#include "engine/Entity.h"

void Entity_Reset(Entity *entity) {
	entity->position = VEC3_ZERO;
	entity->velocity = VEC3_ZERO;
	entity->size = VEC3_ZERO;

	entity->texture = NULL;
}

void Entity_Update(Entity *entity, Game *game, float dt) {
	(void) (game);
	(void) (entity);
	(void) (dt);

	entity->position.x += entity->velocity.x * dt;
	entity->position.y += entity->velocity.y * dt;
	entity->position.z += entity->velocity.z * dt;

	if(entity->update != NULL) {
		entity->update(entity, game, dt);
	}
}
