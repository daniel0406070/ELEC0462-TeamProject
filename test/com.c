#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include "com.h"


void *send_msg(void* arg) {
	thread_arg *args = (thread_arg *)arg;

	
	int sock = args->sock;
	char name[NAME_SIZE];
	strcpy(name, args->name);

	char name_msg[BUFSIZ];
	while(1) {
		fgets(args->msg, BUFSIZ, stdin);
		
		if(!strcmp(args->msg, "q\n") || !strcmp(args->msg, "Q\n")) {
			close(sock);
			exit(0);
		}

		sprintf(name_msg, "%s %s", name, args->msg);

		write(sock, name_msg, strlen(name_msg));
	}
	return NULL;
}

void *recv_msg(void* arg) {
	thread_arg *args = (thread_arg *)arg;

	int sock = args->sock;
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