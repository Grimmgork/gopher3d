#include <gopher.h>
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

struct gopher_item_value {
	int length;
	struct gopher_item_value_chunk* first;
	struct gopher_item_value_chunk* last;
};

struct gopher_item_value_chunk {
	int length;
	char* start;
	struct gopher_item_value_chunk* next;
};

struct gopher_request {
	SOCKET socket;
};

struct gopher_parser {
	int state;
	int row_length;
	int value_length;
	struct gopher_item* first;
	struct gopher_item* last;
};

int
gopher_init()
{
	// initialize winsock
	WSADATA wsadata;
	return WSAStartup(MAKEWORD(2,2), &wsadata);
}

int
gopher_end()
{
	// free winsock
	return WSACleanup();
}

struct gopher_request*
gopher_fetch(char* hostname, char* port, char* path)
{
	struct gopher_request* request = malloc(sizeof(struct gopher_request));
	if (request == NULL) {
		return NULL;
	}

    int result;
	struct addrinfo* info = NULL;
    struct addrinfo* ptr = NULL;
    struct addrinfo hints;

	ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // resolve server address and port
    result = getaddrinfo(hostname, port, &hints, &info);
    if (result != 0) {
        return NULL;
    }

	SOCKET connectionSocket = INVALID_SOCKET;
    // attempt to connect to an address until one succeeds
    for (ptr = info; ptr != NULL; ptr = ptr->ai_next) {

        // create a socket
        connectionSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (connectionSocket == INVALID_SOCKET) {
			freeaddrinfo(info);
            return NULL;
        }

        // try connect to server
        result = connect(connectionSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (result == SOCKET_ERROR) {
            closesocket(connectionSocket);
            connectionSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(info);
    if (connectionSocket == INVALID_SOCKET) {
        return NULL;
    }

    result = send(connectionSocket, path, (int)strlen(path), 0);
    if (result == SOCKET_ERROR) {
        return NULL;
    }

	result = shutdown(connectionSocket, SD_SEND);
	if (result != 0) {
		return NULL;
	}

	request->socket = connectionSocket;
    return request;
}

int
gopher_recv(struct gopher_request* request, void* buffer, int length)
{
	return recv(request->socket, buffer, length, 0);
}

int
gopher_close(struct gopher_request* request)
{
	int result = closesocket(request->socket);
	if (result != 0) {
		return result;
	}
	free(request);
	return 0;
}

int 
gopher_parse(struct arena* arena, struct gopher_parser* parser, char* start, int length)
{

}

struct gopher_item* 
gopher_get_first_item(struct gopher_parser* parser)
{

}

int 
gopher_get_value(struct gopher_item_value* value, void* buffer, int length)
{

}

size_t 
gopher_get_value_size(struct gopher_item_value* value)
{

}