#ifndef BUILDER_H
#define BUILDER_H

#include "engine/Types.h"
#include "base/Memory.h"
#include "renderer/Mesh.h"

#include <stdbool.h>

void Builder_BuildMesh(Memory *stack, World *world);

#endif
