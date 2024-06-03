#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include "com.h"

extern int is_send;

void *send_msg(void* arg) {
	thread_arg *args = (thread_arg *)arg;
	
	int sock = args->sock;
	//char name[NAME_SIZE];
	//strcpy(name, args->name);

	char buf[BUFSIZ];
	while(1) {
		fgets(args->msg, BUFSIZ, stdin);
		
		if(!strcmp(args->msg, "q\n") || !strcmp(args->msg, "Q\n")) {
			close(sock);
			exit(0);
		}
		//send message 
		sprintf(buf, "%d%s", TYPING, args->msg);
		write(sock, buf, strlen(buf));
	}
	return NULL;
}

void *recv_msg(void* arg) {
	thread_arg *args = (thread_arg *)arg;

	int sock = args->sock;
	int str_len;
	
	while(1) {
		str_len = read(sock, name_msg, BUFSIZ);

		if(str_len == -1)
			return (void *) -1;

		name_msg[str_len] = '\0';

		//generate sigalarm
		kill(0, SIGALRM);
	}
	return NULL;
}
