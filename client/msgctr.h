#define _msgctr_h

#ifndef _typectr_h
#include "typectr.h"
#endif

void parse_client_msg(client_message message, char buffer[CONTENT_SIZE]);
client_message parse_to_client_msg(char *msg);


server_message parse_to_server_msg(char *msg);
void parse_server_msg(server_message message, char buffer[CONTENT_SIZE]);