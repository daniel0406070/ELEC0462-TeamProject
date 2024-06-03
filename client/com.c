#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include "com.h"

void *send_msg(void* arg) {
	thread_arg *args = (thread_arg *)arg;

	int sock = args->sock;
	while(*(args->state)) {

		/*
		if(*(args->command) == 0)
			continue;
		else if(*(args->command) == -1) {
			close(sock);
			exit(0);
		}*/
		fgets(args->msg, CONTENT_SIZE, stdin);
		
		if(!strcmp(args->msg, "q\n"))
			break;

		write(sock, args->msg, strlen(args->msg));
		
		//*(args->command) = 0;
	}
	return NULL;
}

void *recv_msg(void* arg) {
	thread_arg *args = (thread_arg *)arg;
	
	int sock = args->sock;
	int str_len;

	while(*(args->state)) {
		str_len = read(sock, args->msg, BUFSIZ);

		if(str_len == -1)
			return (void *) -1;

		args->msg[str_len] = '\0';
		
		//*(args->command) = 1;

		//generate sigalarm
		kill(0, SIGALRM);
	}
	return NULL;
}
