#include "pollSwayer.h"

struct server{
    int portNum;    // Open port number we need to communicate
    char* serverName;   // Server's name we will connect to
};

// Initialize server's values
static void initialize_server(Server server, int portNum, char* serverName){
    server->portNum = portNum;
    strcpy(server->serverName, serverName);
}

Server create_server(int portNum, char* serverName){
    Server server = (Server) mem_allocate(sizeof(struct server));
    server->serverName = (char*) mem_allocate(sizeof(char) * MAXSTRING);
    initialize_server(server, portNum, serverName);
    return server;
}

void destroy_server(Server server){
    mem_free(server->serverName);
    mem_free(server);
}

int get_server_port_number(Server server){
    return server->portNum;
}

char* get_server_name(Server server){
    return server->serverName;
}