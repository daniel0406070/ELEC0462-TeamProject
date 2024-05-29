// -- Default Game Settings --
#define MAX_PLAYER 4
#define ROUND 5
#define CONTENT_SIZE 256

#define BUF_SIZE 100
#define NAME_SIZE 20

typedef struct palyer {
	int score;
	char name[NAME_SIZE];
} player;

// -- Client Settings --
#define JOIN 1
#define START 2
#define TYPING 3

typedef int client_message_type;
typedef struct client_message {
    client_message_type type;
    char content[CONTENT_SIZE];
} client_message;

// -- Server Settings --
#define BROADCAST 1

typedef int server_message_type;
typedef struct server_message {
    server_message_type type;
    char content[CONTENT_SIZE];
} server_message;


