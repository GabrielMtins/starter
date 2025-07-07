#include "base/Memory.h"
#include <stdio.h>

Memory Memory_Create(void *block, size_t size) {
	Memory mems = {block, size, 0};

	return mems;
}

void * Memory_Alloc(Memory *mems, size_t size) {
	char *location;

	if(mems->top >= mems->size)
		return NULL;

	location = mems->block + mems->top;
	mems->top += size;

	return (void *) location;
}

void Memory_Free(Memory *mems) {
	mems->top = 0;
}

void * Memory_GetTop(Memory *mems) {
	return (void *) (mems->block + mems->top);
}

size_t Memory_SaveState(Memory *mems) {
	return mems->top;
}

void Memory_RestoreState(Memory *mems, size_t old_state) {
	mems->top = old_state;
}

const char * Memory_ReadFileAsString(Memory *mems, const char *filename) {
	FILE *file = fopen(filename, "r");
	char *str;
	size_t size;

	if(file == NULL)
		return NULL;

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	str = (char *) Memory_Alloc(mems, size + 1);

	fread(str, 1, size, file);

	str[size] = '\0';

	return (const char *) str;
}
