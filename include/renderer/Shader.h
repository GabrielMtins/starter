#ifndef SHADER_H
#define SHADER_H

#include "base/Mat4.h"

typedef struct {
	unsigned int id;
} Shader;

bool Shader_Load(Shader *shader, const char *vertex_src, const char *fragment_src);

void Shader_Use(const Shader *shader);

bool Shader_SetUniform1i(const Shader *shader, const char *name, int i);

bool Shader_SetUniform3f(const Shader *shader, const char *name, float x, float y, float z);
bool Shader_SetUniform4f(const Shader *shader, const char *name, float x, float y, float z, float w);
bool Shader_SetUniformMat4(const Shader *shader, const char *name, const Mat4 *mat4);

void Shader_Destroy(Shader *shader);

#endif
