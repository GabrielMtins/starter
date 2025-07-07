#ifndef BUILDER_H
#define BUILDER_H

#include "engine/Types.h"
#include "base/Memory.h"
#include "renderer/Mesh.h"

#include <stdbool.h>

bool Builder_BuildMesh(Mesh *mesh, Memory *stack, const World *world);

#endif
