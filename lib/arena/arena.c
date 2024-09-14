#include <stdlib.h>
#include <arena.h>

struct arena_chunk {
	void* start;
	void* end;
	void* alloc;
	struct arena_chunk* next;
};

struct arena {
	struct arena_chunk* start;
	struct arena_chunk* end;
};

struct arena* 
arena_init()
{
	struct arena* arena = malloc(sizeof(arena));
	if (arena == NULL) {
		return NULL;
	}
	arena->start = NULL;
	arena->end = NULL;
	return arena;
}

int
arena_free(struct arena* arena)
{
	if (arena->end = NULL) {
		free(arena);
		return 0;
	}

	struct arena_chunk* next = arena->start;
	do {
		struct arena_chunk* chunkToFree = next;
		next = chunkToFree->next;
		free(chunkToFree);
	} while(next != NULL);
	return 0;
}

struct arena_chunk*
allocate_chunk(size_t size)
{
	// size should be a minimum size
	if (size < ARENA_CHUNK_SIZE) {
		size = ARENA_CHUNK_SIZE;
	}

	size_t allocation_size = sizeof(struct arena_chunk) + size;
	void* allocation = malloc(allocation_size);
	
	if (allocation == NULL) {
		return NULL;
	}

	struct arena_chunk* chunk = (struct arena_chunk*) allocation;
	chunk->start = allocation + sizeof(struct arena_chunk);
	chunk->alloc = chunk->start;
	chunk->end = allocation + allocation_size;
	chunk->next = NULL;

	return chunk;
}

void*
arena_push(struct arena* arena, size_t size)
{
	// create first one
	if (arena->start == NULL)
	{
		struct arena_chunk* chunk = allocate_chunk(size);
		if (chunk == NULL) {
			return NULL;
		}
		chunk->alloc += size;
		arena->start = chunk;
		arena->end = chunk;
		return chunk->start;
	}

	struct arena_chunk* lastChunk = arena->end;
	// append 
	if (lastChunk->alloc + size > lastChunk->end)
	{
		struct arena_chunk* chunk = allocate_chunk(size);
		if (chunk == NULL) {
			return NULL;
		}

		chunk->alloc += size;
		lastChunk->next = chunk;
		arena->end = chunk;
		return chunk->start;
	}
	else
	{
		void* start = lastChunk->alloc;
		lastChunk->alloc += size;
		return start;
	}
}
