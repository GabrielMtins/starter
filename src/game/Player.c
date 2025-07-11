#include "game/Player.h"
#include <math.h>

static void Player_Update(Entity *entity, Game *game, float dt);

void Player_Create(Entity *entity) {
	entity->update = Player_Update;
	entity->position = (Vec3) {3.0f, 0.0f, 3.0f};
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

	entity->direction = (Vec3) {
		sin(entity->angle.y),
		0.0f,
		-cos(entity->angle.y)
	};

	Vec3_Mul(&entity->velocity, &entity->direction, actual_velocity);

	Mat4_RotateY(
			&rotation,
			entity->angle.y
			);

	Mat4_Transform(
			&game->view,
			-entity->position.x,
			-entity->position.y - 0.5f,
			-entity->position.z
			);

	Mat4_Mul(
			&game->view,
			&rotation,
			&game->view
			);
}
