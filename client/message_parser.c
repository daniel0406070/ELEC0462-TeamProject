#include "typeracer.h"
#include <stdlib.h>
#include <string.h>

void clear_buffer(char buffer[CONTENT_SIZE]) {
    for(int i = 0; i < CONTENT_SIZE; i++) {
        buffer[i] = 0;
    }
}

server_message parse_to_server_msg(char *msg) {
    server_message result;
    result.type = msg[0];
    strcpy(result.content, msg);
    return result;
}

void parse_server_msg(server_message message, char buffer[CONTENT_SIZE]) {
    clear_buffer(buffer);
    buffer[0] = message.type;
    strcpy(buffer+1, message.content);
}

void parse_client_msg(client_message message, char buffer[CONTENT_SIZE]) {
    clear_buffer(buffer);
    buffer[0] = message.type;
    strcpy(buffer+1, message.content);
}

client_message parse_to_client_msg(char *msg) {
    client_message result;
    result.type = msg[0];
    strcpy(result.content, msg);
    return result;
}