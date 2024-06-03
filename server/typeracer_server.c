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
#include <signal.h>
#define BUFFER_SIZE 256
#define NAME_SIZE 100
#define PORT 20528
#define TEST 1

void round_alrm(int sig);
void *handle_clnt(void *arg);
void handle_error(char *msg);
void scan_words();
char *pick_random_words();
void send_msg(server_message message);
void received_msg(int client, char *raw_message, int len);
void on_type(int client_id, char *input_sentence);

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
char temp_msg[BUFFER_SIZE];
char sentence[BUFFER_SIZE];
pthread_mutex_t mutex;

int word_size = 0;
char presenting_words[128][BUFFER_SIZE];

int main(int argc, char *argv[]) {
    signal(SIGALRM, round_alrm);
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_sz;
    pthread_t t_id;

    int option;
    socklen_t optlen;
    
    scan_words();
    srand(time(NULL));
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
        message.type = ADDPLAYER;
        strcpy(message.content, name[client_id]);
        send_msg(message);
        return;
    }
    else if (message.type == START) {
        if(ready_game()) {
            start_game();
        }
    }
    else if (message.type == TYPING) {
        on_type(client_id, message.content);
    }
}

void send_msg(server_message message) {
    pthread_mutex_lock(&mutex);
    parse_server_msg(message, temp_msg);
    printf("[%d] msg: %s\n", temp_msg[0], temp_msg);

    int len = strlen(temp_msg);
    for (int i = 0; i < player_count; i++) {
        printf("send to %s\n\n", name[i]);
        write(players[i], temp_msg, len);
    }
    pthread_mutex_unlock(&mutex);
}

void send_msg_to(int client_id, server_message message) {
    pthread_mutex_lock(&mutex);
    parse_server_msg(message, temp_msg);
    printf("[%d] msg: %s\n", temp_msg[0], temp_msg);

    int len = strlen(temp_msg);
    printf("send to %s\n\n", name[client_id]);
    write(players[client_id], temp_msg, len);
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

void next_round() {
    game_round += 1;
    if (game_round > ROUND) {
        end_game();
        return;
    }
    strcpy(sentence, pick_random_words());
    server_message message;
    message.type = BROADCAST;
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "[TypeRacer] : 문장을 입력하세요! [%s]\n", sentence);
    strcpy(message.content, buffer);
    send_msg(message);
}

void round_alrm(int sig) {
    if (sig != SIGALRM) return;
    next_round();
}

void on_type(int client_id, char *input_sentence) {
    int len = strlen(sentence);
    server_message message;
    if (len == 0) {
        message.type = WRONG;
        strcpy(message.content, "게임 진행 중이 아닙니다.");
        send_msg_to(client_id, message);
        return;
    }
    else {
         for (int i = 0; i <= len; i++) {
            if (input_sentence[i] == '\n') {
                input_sentence[i] = '\0';
                break;
            }
        }
        input_sentence[len + 1] = '\0';
        input_sentence=input_sentence+1;
    }
    printf("a: %s\n", sentence);
    printf("b: %s\n", input_sentence);
    if (strcmp(sentence, input_sentence) == 0) {
        pthread_mutex_lock(&mutex);
        points[client_id] += 1;
        pthread_mutex_unlock(&mutex);
        message.type = CORRECT;
        strcpy(message.content, "점수를 획득했습니다!");
        send_msg_to(client_id, message);

        server_message message2;
        message2.type = BROADCAST;
        sprintf(message2.content, "[TypeRacer] %s님이 제일 먼저 문장을 입력했습니다.\n", name[client_id]);
        send_msg(message2);
        if (game_round != ROUND) {
            sprintf(message2.content, "[TypeRacer] 잠시 뒤 [%d] 라운드 시작.\n", game_round + 1);
            send_msg(message2);
        }

        strcpy(sentence, "");
        alarm(3);
    } else {
        message.type = WRONG;
        strcpy(message.content, "문장을 잘못 입력했습니다.");
        send_msg_to(client_id, message);
    }
}

int ready_game() {
    if(TEST){
        printf("player_count: %d\n", player_count);
        printf("state: %d\n\n", state);
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
    strcpy(message.content, "[TypeRacer] : 3초 뒤 게임을 시작합니다..\n");
    send_msg(message);
    alarm(3);
}

void end_game() {
    int max = points[0];
    int winner = 0;
    for (int i = 1; i < MAX_PLAYER; i++) {
        if (points[i] > max) {
            max = points[i];
            winner = i;
        }
    }
    state = READY;
    server_message message;
    message.type = GAMEEND;
    sprintf(message.content, "%s: %d 포인트\n", name[winner], max);
    send_msg(message);
}

void handle_error(char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}