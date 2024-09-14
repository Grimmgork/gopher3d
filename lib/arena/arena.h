#include <stddef.h>
#define ARENA_CHUNK_SIZE 512

struct arena;

struct arena* arena_init();
int arena_free(struct arena* arena);
void* arena_push(struct arena* arena, size_t size);