#include "typeracer.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#define BUFFER_SIZE 128
#define NAME_SIZE 100
#define PORT 20528
#define TEST 1

void *handle_clnt(void *arg);
void handle_error(char *msg);
void scan_words();
char *pick_random_words();
void send_msg(server_message message, int len);
void received_msg(int client, char *raw_message, int len);


// Game
int ready_game();
void start_game();
void end_game();

server_state state = READY;
int game_round = 0;

int player_count = 0;
int players[MAX_PLAYER];
int points[MAX_PLAYER] = { 0 };
char name[MAX_PLAYER][NAME_SIZE] = { 0 };
char msg[BUFFER_SIZE];
pthread_mutex_t mutex;

int word_size = 0;
char presenting_words[BUFFER_SIZE][BUFFER_SIZE];

int main(int argc, char *argv[]) {
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_sz;
    pthread_t t_id;

    int option;
    socklen_t optlen;
    
    scan_words();
    pthread_mutex_init(&mutex, NULL);
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    optlen = sizeof(option);
    option = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void *)&option, optlen);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(PORT);

    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1) {
        handle_error("binding error");
    }
    if (listen(serv_sock, 5) == -1) {
        handle_error("listening error");
    }
    while (1) {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);

        pthread_mutex_lock(&mutex);
        players[player_count++] = clnt_sock;
        pthread_mutex_unlock(&mutex);
        pthread_create(&t_id, NULL, handle_clnt, (void *)&clnt_sock);
        pthread_detach(t_id);
        printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
    }
    close(serv_sock);
    return 0;
}

void *handle_clnt(void *arg) {
    int clnt_sock = *((int *)arg);
    int str_len = 0;
    char msg[BUFFER_SIZE];
    while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0) {
        if(TEST) printf("%s\n", msg);
        received_msg(clnt_sock, msg, str_len);
    }
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < player_count; i++) {
        if (clnt_sock == players[i]) {
            while (i++ < player_count - 1) {
                players[i] = players[i + 1];
            }
            break;
        }
    }
    player_count -= 1;
    pthread_mutex_unlock(&mutex);
    close(clnt_sock);
    return NULL;
}

int get_client_id(int client) {
    for (int i = 0; i < player_count; i++) {
        if(client == players[i]) return i;
    }
    return -1;
}

void received_msg(int client, char *raw_message, int len) {
    client_message message = parse_to_client_msg(raw_message);
    int client_id = get_client_id(client);

    if(TEST){
        printf("client_id: %d\n", client_id);
        printf("client_message: %s\n", message.content);
        printf("client_message_type: %d\n", message.type);
    }

    if(message.type == JOIN) {
        strcpy(name[client_id], message.content);
        server_message message;
        message.type = BROADCAST;
        sprintf(message.content, "[%s] has joined a game", name[client_id]);
        send_msg(message, len);
        return;
    }
    else if (message.type == START) {
        if(ready_game()) {
            start_game();
        }
    }
    else if (message.type == TYPING) {
        
    }
}

void send_msg(server_message message, int len) {
    pthread_mutex_lock(&mutex);
    parse_server_msg(message, msg);
    printf("msg: %s\n", msg);
    for (int i = 0; i < player_count; i++) {
        printf("send to %d\n", players[i]);
        write(players[i], msg, len);
    }
    pthread_mutex_unlock(&mutex);
}

char *pick_random_words() {
    int random = rand() % word_size;
    return presenting_words[random];
}

void scan_words() {
    FILE* file = fopen("presenting_word.txt", "r");
    if (file == NULL) {
        perror("cannot open file");
        exit(1);
    }
    char buffer[BUFFER_SIZE];
    while(fgets(buffer, BUFFER_SIZE, file) != NULL) {
        word_size += 1;
        int len = strlen(buffer);
        if (strcmp(buffer + len - 1, "\n") == 0) {
            buffer[len - 1] = '\0';
        }
        strcpy(presenting_words[word_size - 1], buffer);
    } 
    fclose(file); 
}

int ready_game() {
    if(TEST){
        printf("player_count: %d\n", player_count);
        printf("state: %d\n", state);
    }

    if (player_count < 1) return 0;
    if (state == PLAYING) return 0;
    game_round = 0;
    for (int i = 0; i < MAX_PLAYER; i++) {
        points[i] = 0;
    }
    return 1;
}

void start_game() {
    if(TEST) printf("Game Start\n");

    state = PLAYING;
    server_message message;
    message.type = BROADCAST;
    strcpy(message.content, "[TypeRacer] : 5초 뒤 게임을 시작합니다..");
    send_msg(message, sizeof(msg));
}

void end_game() {
    state = READY;
}

void handle_error(char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}