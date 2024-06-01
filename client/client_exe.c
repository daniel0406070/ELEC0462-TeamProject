#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>
#include <curses.h>

#include "typeracer.h"

#define START_GAME	"start game"
#define END_GAME	"end game"

typedef struct thread_arg {
	int sock;
	char name[NAME_SIZE];
}thread_arg; 

player players[4];
int player_count;

int left_edge = 5;
int right_edge = 35;
int top_row = 5;
int bot_row = 55;

char buffer[BUFSIZ];
char name[NAME_SIZE];
char msg[BUFSIZ];
char word[BUFSIZ];

void setupGame(int);
void error_handling(char*);

void *send_msg(void* arg);
void *recv_msg(void* arg);

void displayInGame();
char* toStringPlayer(player p);
char* getSpliter(char elem, int left, int right);

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
	setupGame(sock);
	
	//스레드 생성 메세지 송/수신용
	pthread_create(&snd_thread, NULL, send_msg, (void *)&sock);
	pthread_create(&rcv_thread, NULL, recv_msg, (void *)&sock);

	pthread_join(snd_thread, &thread_return);
	pthread_join(rcv_thread, &thread_return);

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

void *send_msg(void* arg) {
	thread_arg *args = (thread_arg *)arg;
	
	int sock = args->sock;
	char name[NAME_SIZE];
	strcpy(name, args->name);

	char name_msg[BUFSIZ];
	while(1) {
		fgets(msg, BUFSIZ, stdin);
		
		if(!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) {
			close(sock);
			exit(0);
		}

		sprintf(name_msg, "%s %s", name, msg);

		write(sock, name_msg, strlen(name_msg));
	}
	return NULL;
}

void *recv_msg(void* arg) {
	int sock = *((int *)arg);
	char name_msg[BUFSIZ];

	int str_len;
	
	while(1) {
		str_len = read(sock, name_msg, BUFSIZ);

		if(str_len == -1)
			return (void *) -1;

		name_msg[str_len] = '\0';

		/*
		 * 이러쿵 저러쿵
		 */
	}
	return NULL;
}

void displayInGame() {
	int r, c;
	char* line;
	
	//set row col
	r = top_row;
	c = left_edge;
	
	//clear screen
	clear();
	
	//display players
	for(int i = 0; i < player_count; i++) {
		line = toStringPlayer(players[i]);

		move(r, c);
		addstr(line);

		r++;
	}
	//get spliter like "---- ... ----" form
	line = getSpliter('-', left_edge, right_edge);
	
	//move next line and display spliter
	move(r, c);
	addstr(line);
	
	//move down 2 row and print target word
	r += 2;
	move(r, c);
	addstr(word);
	
	//move to bot_row - 5 and display spliter
	r = bot_row -5;
	move(r, c);
	addstr(line);
	
	//move next line
	r++;
}

char* toStringPlayer(player p) {
	char* ret = malloc(200);

	sprintf(ret, "%20s %d", p.name, p.score);

	return ret;
}

char* getSpliter(char elem, int left, int right) {
	int length = right - left;
	char* ret = malloc(length);
	
	int i = 0;
	for(; i < length; i++)
		ret[i] = elem;
	ret[i] = '\n';

	return ret;
}
