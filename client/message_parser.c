#include "typeracer.h"
#include <stdlib.h>
#include <string.h>

server_message parse_to_server_msg(char *msg) {
    server_message result;
    result.type = msg[0] - '0';
    strcpy(result.content, msg + 1);
    return result;
}

void parse_server_msg(server_message message, char buffer[CONTENT_SIZE]) {
    buffer[0] = message.type;
    strcpy(buffer, message.content + 1);
}

void parse_client_msg(client_message message, char buffer[CONTENT_SIZE]) {
    buffer[0] = message.type;
    strcpy(buffer, message.content + 1);
}

client_message parse_to_client_msg(char *msg) {
    client_message result;
    result.type = msg[0] - '0';
    strcpy(result.content, msg + 1);
    return result;
}