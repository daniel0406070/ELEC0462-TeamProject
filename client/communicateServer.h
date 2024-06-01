#define NAME_SIZE 20

typedef struct thread_arg {
	int sock;
	char name[NAME_SIZE];
}thread_arg; 

void *send_msg(void* arg);

void *recv_msg(void* arg);
