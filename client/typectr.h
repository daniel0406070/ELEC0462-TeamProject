#define _typectr_h

#ifndef _totalset_h
#include "totalset.h"
#endif

typedef struct player {
	int score;
	char name[NAME_SIZE];
} player;

typedef int client_message_type;
typedef struct client_message {
	client_message_type type;
	char content[CONTENT_SIZE];
} client_message;


typedef int server_state;
typedef int server_message_type;
typedef struct server_message {
	server_message_type type;
	char content[CONTENT_SIZE];
} server_message;

// Thread argument
typedef struct thread_arg {
	int sock;
	int* state;
	
	int* command; //0 == idle, 1 is something
	char* msg;
}thread_arg;
