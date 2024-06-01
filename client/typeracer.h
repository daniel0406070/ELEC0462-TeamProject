#include <string.h>
#include <stdlib.h>

// -- Default Game Settings --
#define MAX_PLAYER 4
#define ROUND 5
#define CONTENT_SIZE 128

// -- Client Settings --
#define JOIN 1
#define START 2
#define TYPING 3

typedef int client_message_type;
typedef struct client_message {
    client_message_type type;
    char content[CONTENT_SIZE];
} client_message;

void parse_client_msg(client_message message, char buffer[CONTENT_SIZE]);
client_message parse_to_client_msg(int type, char *msg);

// -- Server Settings --
#define BROADCAST 1

// -- Game State
#define READY 0
#define PLAYING 1

typedef int server_state;
typedef int server_message_type;
typedef struct server_message {
    server_message_type type;
    char content[CONTENT_SIZE];
} server_message;

server_message parse_to_server_msg(int type, char *msg);
void parse_server_msg(server_message message, char buffer[CONTENT_SIZE]);


