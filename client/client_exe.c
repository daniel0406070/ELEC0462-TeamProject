#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>

#include "typeracer.h"
#include "displayScreen.h"
#include "communicateServer.h"

#define START	"start game"
#define END	"end game"

player players[4];
int player_count;

char buffer[BUFSIZ];
char name[NAME_SIZE];

void setupGame(int);
void error_handling(char*);

int main(int argc, char* argv[]) {
	int sock;
	struct sockaddr_in serv_addr;

	pthread_t snd_thread, rcv_thread;

	void *thread_return;
	
	if(argc != 4) {
		printf("Usage: %s <IP> <port> <name>\n", argv[0]);
		exit(1);
	}

	sprintf(name, "[%s]", argv[3]);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));
	
	//signal 발생시 화면 갱신
	signal(SIGALRM, displayInGame);
	
	/*
	 * 서버한테 이름 전송 후 답변 기다림
	 * start 신호 기다림
	 */
	setupGame(sock, name);
	
	//스레드 생성 메세지 송/수신용
	pthread_create(&snd_thread, NULL, send_msg, (void *)&sock);
	pthread_create(&rcv_thread, NULL, recv_msg, (void *)&sock);

	pthread_join(snd_thread, &thread_return);
	pthread_join(rcv_thread, &thread_return);

	close(sock);
	
	return 0;
}

void setupGame(int sock) {
	//서버에게 이름 전송
	write(sock, name, strlen(name));
	
	while(1) {
		read(sock, buffer, BUFSIZ);
		
		//게임이 시작한다면
		if(!strcmp(buffer, START)) 
			break;
	}

	/*
	 * 유저정보 담아서 저장
	 */
}

void error_handling(char* msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
