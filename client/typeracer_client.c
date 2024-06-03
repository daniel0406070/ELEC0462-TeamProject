#include "typeracer.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUFFER_SIZE 256
#define NAME_SIZE 20

int point = 0;

void *send_msg(void *arg);
void *recv_msg(void *arg);
void handle_error(char *msg);

char name[NAME_SIZE] = "";
char temp_msg[BUFFER_SIZE];

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t snd_thread, rcv_thread;
    void *thread_return;
    if (argc != 4) {
        printf("Usage : %s <IP> <port> <name>\n", argv[0]);
        exit(1);
    }

    sprintf(name, "[%s]", argv[3]);

    sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        handle_error("connect() error");
    }

    pthread_create(&snd_thread, NULL, send_msg, (void *)&sock);
    pthread_create(&rcv_thread, NULL, recv_msg, (void *)&sock);

    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);
    close(sock);
    return 0;
}

void *send_msg(void *arg) {
    int sock = *((int *)arg);
    char raw_msg[BUFFER_SIZE];
    client_message join_message;
    join_message.type = JOIN;
    strcpy(join_message.content, name);
    parse_client_msg(join_message, raw_msg);
    write(sock, raw_msg, strlen(raw_msg));
    while (1) {
        fgets(temp_msg, BUFFER_SIZE, stdin);
        client_message message;
        if (!strcmp(temp_msg, "q\n") || !strcmp(temp_msg, "Q\n")) {
            close(sock);
            exit(0);
        }
        else if(!strcmp(temp_msg, "start\n")) {
            message.type = START;
            strcpy(message.content, "start the game!");
            point = 0;
        } else {
            message.type = TYPING;
            strcpy(message.content, temp_msg);
        }
        parse_client_msg(message, raw_msg);
        write(sock, raw_msg, strlen(raw_msg));
    }
    return NULL;
}

void *recv_msg(void *arg) {
    int sock = *((int *)arg);
    char raw_msg[BUFFER_SIZE];
    int str_len;
    while (1) {
        str_len = read(sock, raw_msg, BUFFER_SIZE - 1);
        if (str_len == -1)
            return (void *)-1;
        raw_msg[str_len] = 0;
        server_message message = parse_to_server_msg(raw_msg);
        if (message.type == BROADCAST) {
            printf("%s", message.content);
        } else if (message.type == ADDPLAYER) {
            printf("%s has joined the game.\n", message.content);
        } else if (message.type == WRONG) {
            printf("%s\n", message.content);
        } else if (message.type == CORRECT) {
            point += 1;
            printf("%s\n", message.content);
            printf("현재 포인트 : %d\n", point);
        } else if (message.type == GAMEEND) {
            printf("[TypeRacer] 게임이 종료되었습니다!\n");
            printf("승리:\n");
            printf("%s\n", message.content);
        }
    }
    return NULL;
}

void handle_error(char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}