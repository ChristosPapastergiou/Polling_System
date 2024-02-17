#ifndef POLLSWAYER
#define POLLSWAYER

/**** Includes ****/

#include "../../Poller/include/poller.h"

/**** Defines ****/

#define STRTOK_DELIMITER " "

/**** Typedefs ****/

typedef struct server* Server;
typedef struct guide_args* Guide_args;

/**** guideThread.c ****/

// Creates guide's thread arguments and returns it
Guide_args create_guide_args(Server server, char* name, char* party);

// Destroying guide's arguments
void destroy_guide_args(Guide_args guide_args);

// Guide thread function 
Pointer guide_thread(Pointer guide_args);

/**** server.c ****/

// Creating server with server's informations
Server create_server(int portNum, char* serverName);

// Destroying the server
void destroy_server(Server server);

// Returns the port number of the server
int get_server_port_number(Server server);

// Returns the name of the server
char* get_server_name(Server server);

#endif