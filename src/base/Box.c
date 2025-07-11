#include "base/Box.h"

bool Box_CheckCollision(const Vec3 *a_pos, const Vec3 *a_size, const Vec3 *b_pos, const Vec3 *b_size) {
	if(a_pos->x + a_size->x < b_pos->x) 
		return false;

	if(a_pos->y + a_size->y < b_pos->y) 
		return false;

	if(a_pos->z + a_size->z < b_pos->z) 
		return false;

	if(b_pos->x + b_size->x < a_pos->x) 
		return false;

	if(b_pos->y + b_size->y < a_pos->y) 
		return false;

	if(b_pos->z + b_size->z < a_pos->z) 
		return false;

	return true;
}
