#ifndef TEXTURE_H
#define TEXTURE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* Usada para texture atlas */
typedef struct {
	unsigned int texture_id;
	int w, h;
} MegaTexture;

/* Usada para texture array */
typedef struct {
	unsigned int texture_id;
	int w, h;
	size_t size;
} TextureArray;

typedef struct {
	MegaTexture *mega_texture;

	int x, y;
	int cell_w, cell_h;
	int num_cells_x;
} Texture;

bool Texture_LoadDefaults(void);

bool TextureArray_Create(TextureArray *texture_array, int w, int h);

bool TextureArray_Load(TextureArray *texture_array, const char *filename);

bool TextureArray_Use(TextureArray *texture_array, int unit);

bool MegaTexture_Load(MegaTexture *mega_texture, const char *filename);

bool MegaTexture_Destroy(MegaTexture *mega_texture);

bool Texture_Create(Texture *texture, MegaTexture *mega_texture, int x, int y, int w, int h, int cell_w, int cell_h);

bool Texture_Render(Texture *texture, int x, int y, int id, int flip, int screen_width, int screen_height);

#endif
