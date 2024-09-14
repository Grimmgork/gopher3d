#include <arena.h>
#include <stddef.h>

struct gopher_request;
struct gopher_parser;
struct gopher_item_value;
struct gopher_item_value_chunk;

struct gopher_item {
	char type;
	struct gopher_item_value* host;
	struct gopher_item_value* port;
	struct gopher_item_value* path;
	struct gopher_item_value* text;
	struct gopher_item* next;
};

int gopher_init();

int gopher_end();

struct gopher_request* gopher_fetch(char* hostname, char* port, char* path);

int gopher_recv(struct gopher_request* request, void* buffer, int length);

int gopher_close(struct gopher_request* request);

int gopher_parse(struct arena* arena, struct gopher_parser* parser, char* start, int length);

struct gopher_item* gopher_get_first_item(struct gopher_parser* parser);

int gopher_get_value(struct gopher_item_value* value, void* buffer, int length);

size_t gopher_get_value_size(struct gopher_item_value* value);
