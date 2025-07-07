#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include <glad/glad.h>

#include "base/Memory.h"

enum keys_e {
	INPUT_LEFT = 0,
	INPUT_DOWN,
	INPUT_UP,
	INPUT_RIGHT,
	INPUT_JUMP,
	INPUT_CRAWL,
	INPUT_FIRE1,
	INPUT_FIRE2,
	NUM_KEYS
};

typedef struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_RWops *data;
	SDL_GLContext gl_context;

	int width, height;

	size_t tick;
	size_t delta_tick;
	int mouse_xrel, mouse_yrel;
	float dt;

	size_t min_time_frame;

	bool quit;

	int *key_mapping;
	int *button_mapping;

	Memory *memory;
	Memory *stack;

	const uint8_t *keyboard_state;
	SDL_GameController *controller;
} Context;

Context *Context_Create(const char *title, int w, int h, Memory *memory, Memory *stack);

bool Context_OpenGameController(Context *context, int joystick_index);

bool Context_GetKey(Context *context, int key);

bool Context_SetFps(Context *context, size_t fps);

bool Context_SetDataFromMem(Context *context, const char *data, int size);

bool Context_SetDataFromFile(Context *context, const char *filename);

void Context_DelayFPS(Context *context);

void Context_PollEvent(Context *context);

bool Context_Destroy(Context *context);

#endif
