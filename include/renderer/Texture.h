#ifndef TEXTURE_H
#define TEXTURE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* Usada para texture array */
typedef struct {
	unsigned int texture_id;
	int w, h;
	size_t size;
} TextureArray;

typedef struct {
	unsigned int texture_id;

	int cell_w, cell_h;
	int num_cells_x;
	int w, h;
	
	int max_id;
} Texture;

bool Texture_LoadDefaults(void);

bool TextureArray_Create(TextureArray *texture_array, int w, int h);

bool TextureArray_Load(TextureArray *texture_array, const char *filename);

bool TextureArray_Use(TextureArray *texture_array, int unit);

bool Texture_Load(Texture *texture, const char *filename, int cell_w, int cell_h);

bool Texture_Use(const Texture *texture, unsigned int unit);

bool Texture_Render(Texture *texture, int x, int y, int id, int flip, int screen_width, int screen_height);

#endif
