#include "game/Player.h"
#include <math.h>

static void Player_Update(Entity *entity, Game *game, float dt);

void Player_Create(Entity *entity) {
	entity->update = Player_Update;
	entity->position = (Vec3) {3.0f, 0.1f, 3.0f};
	entity->size = (Vec3) {0.8f, 0.8f, 0.8f};

	entity->collision_mask = 1;
}

static void Player_Update(Entity *entity, Game *game, float dt) {
	Context *context = game->context;
	const float turning_velocity = 5.0f;
	const float running_velocity = 5.0f;
	float actual_velocity = 0.0f;
	Mat4 rotation;
	
	if(Context_GetKey(context, INPUT_LEFT)) {
		entity->angle.y -= turning_velocity * dt;
	}
	
	if(Context_GetKey(context, INPUT_RIGHT)) {
		entity->angle.y += turning_velocity * dt;
	}

	if(Context_GetKey(context, INPUT_UP)) {
		actual_velocity = running_velocity;
	}

	if(Context_GetKey(context, INPUT_DOWN)) {
		actual_velocity = -running_velocity;
	}

	if(Context_GetKey(context, INPUT_JUMP) && entity->on_floor) {
		entity->velocity.y = 5.0f;
	}

	entity->direction = (Vec3) {
		sin(entity->angle.y),
		0.0f,
		-cos(entity->angle.y)
	};

	float old = entity->velocity.y;

	Vec3_Mul(&entity->velocity, &entity->direction, actual_velocity);

	entity->velocity.y = old - 10.0f * dt;

	Mat4_RotateY(
			&rotation,
			entity->angle.y
			);

	Mat4_Transform(
			&game->view,
			-entity->position.x - entity->size.x / 2,
			-entity->position.y - 0.5f,
			-entity->position.z - entity->size.z / 2
			);

	Mat4_Mul(
			&game->view,
			&rotation,
			&game->view
			);
}
