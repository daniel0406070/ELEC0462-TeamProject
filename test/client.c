#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>
#include <curses.h>

#include "client.h"

#define START_GAME	"start game"
#define END_GAME	"end game"

player players[MAX_PLAYER]; 
int player_count;

char buffer[BUFSIZ];
char msg[BUFSIZ];
char name[NAME_SIZE];
char word[BUFSIZ];

void setupGame(int);
void error_handling(char*);

int pid;


int main(int argc, char* argv[]) {
	int sock;
    thread_arg args;
	struct sockaddr_in serv_addr;
	
	pthread_t send_thread, recive_thread;

	void *thread_return;
	
	if(argc != 4) {
		printf("Usage: %s <IP> <port> <name>\n", argv[0]);
		exit(1);
	}

	pid = getpid();

	sprintf(name, "[%s]", argv[3]);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));
	
	// initscr();

	//signal 발생시 화면 갱신 player* players, int player_count, char* word
	signal(SIGALRM, displayInGame); 
	
	kill(0, SIGALRM);

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1)
        error_handling("socket() error");
    
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");
    
    client_message client_msg;
    client_msg.type = START;
    strcpy(client_msg.content, name);

    parse_client_msg(client_msg, msg);
    printf("name: %s\n", name);
    printf("msg: %s\n", msg);
    write(sock, msg, strlen(msg));

    server_message server_msg;
    while(1) {
        printf("read\n");
        read(sock, buffer, BUFSIZ);
        server_msg = parse_to_server_msg(buffer);
        
        printf("server_msg: %d\n", server_msg.type);
        printf("server_msg: %s\n", server_msg.content);

    }



	/*
	 * 서버한테 이름 전송 후 답변 기다림
	 * start 신호 기다림
	 */
	//setupGame(sock);
	
	//스레드 생성 메세지 송/수신용
    args.sock = sock;
    args.msg = msg;
    strcpy(args.name, name);

	pthread_create(&send_thread, NULL, send_msg, (void *)&args);
	pthread_create(&recive_thread, NULL, recv_msg, (void *)&args);

	pthread_join(send_thread, &thread_return);
	pthread_join(recive_thread, &thread_return);

	close(sock);
	
	return 0;
}

void error_handling(char* msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

void setupGame(int sock) {
	//서버에게 이름 전송
	write(sock, name, strlen(name));
	
	while(1) {
		read(sock, buffer, BUFSIZ);
		
		//게임이 시작한다면
		if(!strcmp(buffer, START_GAME)) 
			break;
	}

	/*
	 * 유저정보 담아서 저장
	 */
}
