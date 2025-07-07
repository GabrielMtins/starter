#ifndef MEMS_H
#define MEMS_H

#include <stddef.h>

/* Stack de alocação de memória.
 * Evitar usar malloc e fragmentar a memória. */

typedef struct {
	char *block;
	size_t size;
	size_t top;
} Memory;

Memory Memory_Create(void *block, size_t size);

void * Memory_Alloc(Memory *mems, size_t size);

void Memory_Free(Memory *mems);

void * Memory_GetTop(Memory *mems);

size_t Memory_SaveState(Memory *mems);

void Memory_RestoreState(Memory *mems, size_t old_state);

const char * Memory_ReadFileAsString(Memory *mems, const char *filename);

#endif
