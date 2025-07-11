#ifndef BOX_H
#define BOX_H

#include <stdbool.h>
#include "Vec3.h"

bool Box_CheckCollision(const Vec3 *a_pos, const Vec3 *a_size, const Vec3 *b_pos, const Vec3 *b_size);

#endif
