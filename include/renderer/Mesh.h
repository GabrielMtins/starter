#ifndef MESH_H
#define MESH_H

#include <stddef.h>
#include <stdint.h>

#include <renderer/Shader.h>
#include <base/Vec3.h>

typedef struct {
	Vec3 position;

	struct {
		float x, y;
	} uv;

	Vec3 normal;
	Vec3 color;
	float layer_index;
} Vertex;

typedef struct {
	unsigned int vao, vbo, ebo;
	unsigned int num_indices;
} Mesh;

bool Vertex_CreateSimple(Vertex *vertex, float x, float y, float z, float u, float v);

bool Mesh_Create(Mesh *mesh, const Vertex *vertices, size_t num_vertices, const unsigned int *indices, size_t num_indices);

bool Mesh_BuildUnitTetrahedron(Mesh *mesh);

bool Mesh_Render(const Mesh *mesh, const Shader *shader);

bool Mesh_Destroy(const Mesh *mesh);

#endif
