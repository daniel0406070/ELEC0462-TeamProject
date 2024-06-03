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

#define TEST 0

//display state
#define IDLE   0
#define INGAME 1
#define RESULT 2

//used in thread
int do_send, do_recv;
char send_buffer[CONTENT_SIZE];
char recv_buffer[CONTENT_SIZE];

//use in here
player players[MAX_PLAYER]; 
int player_count;

client_message client_msg;
server_message server_msg;

char name[NAME_SIZE];

char buffer[BUFSIZ];
char word[CONTENT_SIZE];

int display_state = IDLE;
int connect_state = 1;

pthread_t send_thread, recive_thread;

void setupGame(int);
void sigalarm_handler();
void terminate_thread(pthread_t);
player get_player(char* name);
void error_handling(char*);

int main(int argc, char* argv[]) {
	int sock;
	thread_arg send_args, recv_args;
	struct sockaddr_in serv_addr;

	void *thread_return;
	
	if(argc != 4) {
		printf("Usage: %s <IP> <port> <name>\n", argv[0]);
		exit(1);
	}
	strcpy(name, argv[3]);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));
	
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
	    error_handling("socket() error");
	
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
	    error_handling("connect() error");
	
	/*
	 * 서버한테 이름 전송 후 답변 기다림
	 * start 신호 기다림
	 */
	setupGame(sock);
	
	//signal 발생시 화면 갱신 player* players, int player_count, char* word
	signal(SIGALRM, sigalarm_handler); 
	
	//스레드 생성 메세지 송신용
	send_args.sock = sock;
	send_args.state = &connect_state;
	send_args.command = &do_send;
	send_args.msg = send_buffer;
	
	//스레드 생성 메세지 수신용
	recv_args.sock = sock;
	recv_args.state = &connect_state;
	recv_args.command = &do_recv;
	recv_args.msg = recv_buffer;
	
	//스레드 생성
	pthread_create(&send_thread, NULL, send_msg, (void *)&send_args);
	pthread_create(&recive_thread, NULL, recv_msg, (void *)&recv_args);
	
	/*
	//유저 입력 받음
	while(1) {
		fgets(buffer, CONTENT_SIZE, stdin);
		
		client_msg.type = TYPING; 
		strcpy(client_msg.content, buffer);
		
		parse_client_msg(client_msg, send_buffer);

		if(!strcmp(buffer, "q\n") || !strcmp(buffer, "Q\n"))
			break;
	}
	*/
	//kill(0, SIGALRM);
	
	pthread_join(send_thread, &thread_return);
	pthread_join(recive_thread, &thread_return);

	close(sock);
	
	return 0;
}

//서버에서 메세지가 왔을때
void sigalarm_handler() {
	/*
	server_msg = parse_to_server_msg(recv_buffer);
	char display_msg[CONTENT_SIZE];
	switch(server_msg.type) {
		case 1: 
			strcpy(display_msg, server_msg.content); 
			display_state = INGAME;
			break;
		case 2: 
			strcpy(display_msg, "틀렸습니다.");
			break;
		case 3: 
			if(!strcpy(server_msg.content, name))
				strcpy(display_msg, "맞았습니다!");
			else
				sprintf(display_msg, "%s님이 맞췄습니다!");

			display_state = INGAME;
			break;
		case 4: 
			update_point(); 
			display_state = INGAME; 
			break;
		case 5: 
			display_state = RESULT;
			break;
	}
	switch(display_state) {
		case 1: displayInGame(players, display_msg); break;
		case 2: displayResult(); break;
	}
	*/
	display_state = IDLE;
}

void error_handling(char* msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

void setupGame(int sock) {
	//서버에게 이름 전송
	client_msg.type = JOIN;
	strcpy(client_msg.content, name);
	
	//파싱후 전송
	parse_client_msg(client_msg, send_buffer);
	write(sock, send_buffer, strlen(send_buffer));

	int str_len;
	while(1) {
		str_len = read(sock, buffer, CONTENT_SIZE);
		server_msg = parse_to_server_msg(buffer);
		
		printf("read\n");
		printf("server_msg_type: %d\n", server_msg.type);
		printf("server_msg_content: %s\n", server_msg.content);
		
		if(!strcmp(server_msg.content, "start")) {
			break;
		}
	}
	display_state = INGAME;
	
	initscr();
}

player get_player(char* name) {
	player ret;
	strcpy(ret.name, name);
	return ret;
}

