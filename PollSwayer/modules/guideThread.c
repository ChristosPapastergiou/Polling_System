#include "pollSwayer.h"

struct guide_args{
    char* name;     // Name of the person
    char* party;    // Name of the voted party 
    Server server;  // Server infos
};

// Initialize guide's arguments
static void initialize_guide_args(Guide_args guide_args, Server server, char* name, char* party){
    strcpy(guide_args->name, name);
    strcpy(guide_args->party, party);
    guide_args->server = server;
}

Guide_args create_guide_args(Server server, char* name, char* party){
    Guide_args guide_args = (Guide_args) mem_allocate(sizeof(struct guide_args));
    guide_args->name = (char*) mem_allocate(sizeof(char) * MAXSTRING);
    guide_args->party = (char*) mem_allocate(sizeof(char) * MAXSTRING);
    initialize_guide_args(guide_args, server, name, party);
    return guide_args;
}

void destroy_guide_args(Guide_args guide_args){
    mem_free(guide_args->party);
    mem_free(guide_args->name);
    mem_free(guide_args);
}

Pointer guide_thread(Pointer guide_args){
    Guide_args arguments = (Guide_args) guide_args;

    struct hostent* rem;
    struct sockaddr_in server;
    struct sockaddr* serverptr = (struct sockaddr*) &server;

    char* message = (char*) mem_allocate(sizeof(char) * MAXSTRING); // Allocating temporary space for the message i will get from server

    int server_socket;
    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){    // Creating a socket (using AF_INET domain)
    	perror("server socket creation failure"); 
        mem_free(message);
        exit(EXIT_FAILURE); 
    }

    if((rem = gethostbyname(get_server_name(arguments->server))) == NULL){  // Find server address
	    perror("get host by name failure"); 
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;    // AF_INET domain
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(get_server_port_number(arguments->server)); // Server port

    if(connect(server_socket, serverptr, sizeof(server)) < 0){  // Initiate connection
	    perror("connect failure");
        exit(EXIT_FAILURE);
    }

    if(read(server_socket, message, MAXSTRING - 1) < 0){    // Ready to take the first message 
        perror("read failure");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    clean_string(message);
    if(strcmp(message, "SEND NAME PLEASE") == 0){   // If it is a name then the only think to do is to 
        if(write(server_socket, arguments->name, strlen(arguments->name) + 1) < 0){ // write it the name to server
            perror("write failure");
            close(server_socket);
            exit(EXIT_FAILURE);
        }
    }

    if(read(server_socket, message, MAXSTRING - 1) < 0){    // Ready to take the second message 
        perror("read failure");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    clean_string(message);
    if(strcmp(message, "ALREADY VOTED") == 0){  // If name exists then no more votes
        mem_free(message);
        close(server_socket);
        pthread_exit((Pointer) EXIT_SUCCESS);
    }else if(strcmp(message, "SEND VOTE PLEASE") == 0){ // But if it is not it means the person can vote so
        if(write(server_socket, arguments->party, strlen(arguments->party) + 1) < 0){   // write it the name to server
            perror("write failure");
            close(server_socket);
            exit(EXIT_FAILURE);
        }
    }

    if(read(server_socket, message, MAXSTRING - 1) < 0){    // Ready to take the second message 
        perror("read failure");
        exit(EXIT_FAILURE);
    }
    clean_string(message);
    char* string = (char*) mem_allocate(sizeof(char) * MAXSTRING);
    sprintf(string, "VOTE for Party %s RECORDED", arguments->party);    
    if(strcmp(message, string) == 0){   // If it matches the final message teminate
        mem_free(string);
        mem_free(message);
        close(server_socket);
        shutdown(server_socket, SHUT_RDWR); // Shutdown for safety
        pthread_exit((Pointer) EXIT_SUCCESS);   // And terminate
    }

    mem_free(string);
    mem_free(message);
    
    close(server_socket);

    pthread_exit((Pointer) EXIT_SUCCESS);
}