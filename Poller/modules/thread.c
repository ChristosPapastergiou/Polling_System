#include "poller.h"

int sig_flag = 0; 

pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;
pthread_mutex_t buf_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t thread_mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex for workers to write name-vote on pollPol file

struct worker_args{
    Buffer buffer;  // The sharing buffer 
    Voters voters;  // Voters
    FILE* log_file; // The file that saves the names and party votes
};

struct master_args{
    int portnum;    // The port number of our connection
    Buffer buffer;      // The sharing buffer
    int numWorkerthreads;   // How many workers master thread will create
    Worker_args worker_args;    // Worker arguments
};

// Setting flag to value 1 when ctrl-c come to exit program
static void signal_handler(int signum){
    sig_flag = 1;                       // The flag is set to 1 if ctrl-c is pressed
    pthread_cond_broadcast(&not_full);  // Broadcast to free all the
    pthread_cond_broadcast(&not_empty); // conditional variables
}

// Initialize worker thread arguments
static void initialize_worker_args(Master_args master_args, Buffer buffer, Voters voters, FILE* log_file){
    master_args->worker_args->buffer = buffer;
    master_args->worker_args->voters = voters;
    master_args->worker_args->log_file = log_file;
}

// Creates worker thread arguments
static Worker_args create_worker_args(){
    Worker_args worker_args = (Worker_args) mem_allocate(sizeof(struct worker_args));   // Allocate memory for the worker arguments
    return worker_args;
}

// Initialize master thread arguments 
static void initialize_master_args(Master_args master_args, Buffer buffer, Voters voters, int portnum, int numWorkerthreads, FILE* log_file){
    master_args->buffer = buffer;
    master_args->portnum = portnum;
    master_args->numWorkerthreads = numWorkerthreads; 
    initialize_worker_args(master_args, buffer, voters, log_file);
}

Master_args create_master_args(Buffer buffer, Voters voters, int portnum, int numWorkerthreads, FILE* log_file){
    Master_args master_args = (Master_args) mem_allocate(sizeof(struct master_args));
    master_args->worker_args = create_worker_args();                                            // Master creates both master's arguments
    initialize_master_args(master_args, buffer, voters, portnum, numWorkerthreads, log_file);   // and worker's arguments and also initialize them 
    return master_args;
}

void destroy_master_args(Master_args master_args){
    mem_free(master_args->worker_args);
    mem_free(master_args);
}

Pointer worker_thread(Pointer worker_args){
    Worker_args arguments = (Worker_args) worker_args;

    sigset_t sigset;                            // Setting our handler to ignore
    sigemptyset(&sigset);                       // ctrl-c signal because worker
    sigaddset(&sigset, SIGINT);                 // thread must go on until master
    pthread_sigmask(SIG_BLOCK, &sigset, NULL);  // gets this signal

    char* name = (char*) mem_allocate(sizeof(char) * MAXSTRING);    // Allocating temporary space for
    char* vote = (char*) mem_allocate(sizeof(char) * MAXSTRING);    // name and party vote for every user
    
    int socket_fd;
    while(1){
        if((socket_fd = obtain_from_buffer(arguments->buffer)) == -1){  // Worker thread needs socket's file descriptor to go on
            break;
        }
        
        if(write(socket_fd, NAME, strlen(NAME) + 1) < 0){   // Write the message on screen
            perror("write failure");
            close(socket_fd);
            exit(EXIT_FAILURE);
        }
        if(read(socket_fd, name, MAXSTRING - 1) < 0){       // Read the name that will (or not) vote
            perror("read failure");
            close(socket_fd);
            exit(EXIT_FAILURE);
        }
        clean_string(name);

        if(strlen(name) == 0){
            printf("Need name to be able to vote\n");
            continue;
        }

        pthread_mutex_lock(&thread_mutex);
        if(check_if_already_voted(arguments->voters, name) == true){    // Check if that person already voted
            if(write(socket_fd, VOTED, strlen(VOTED) + 1) < 0){         // In this case show it to me and terminate
                perror("write failure");
                close(socket_fd);
                exit(EXIT_FAILURE);
            }
            pthread_mutex_unlock(&thread_mutex);
            close(socket_fd);
            continue;
        }
        pthread_mutex_unlock(&thread_mutex);

        pthread_mutex_lock(&thread_mutex);
        insert_voter_name(arguments->voters, name);         // If the person hasnt vote register the name
        if(write(socket_fd, VOTE, strlen(VOTE) + 1) < 0){   // And write on screen the message
            perror("write failure");
            close(socket_fd);
            exit(EXIT_FAILURE);
        }
        if(read(socket_fd, vote, MAXSTRING - 1) < 0){   // Read what he vote
            perror("read failure");
            close(socket_fd);
            exit(EXIT_FAILURE);
        }
        clean_string(vote);

        insert_voted_party(arguments->voters, vote);    // Register person's choice

        fprintf(arguments->log_file, "%s\n", connect_strings(name, vote));
        fflush(arguments->log_file);
        pthread_mutex_unlock(&thread_mutex);   // Now another worker is free to register into pollLog another data

        char* string = (char*) mem_allocate(sizeof(char) * MAXSTRING);
        sprintf(string, PARTY, vote);
        if(write(socket_fd, string, strlen(string) + 1) < 0){   // Write on screen the message
            perror("write failure");
            close(socket_fd);
            exit(EXIT_FAILURE);
        }
        mem_free(string);

        close(socket_fd);
    }

    mem_free(name);
    mem_free(vote);

    pthread_exit((Pointer) EXIT_SUCCESS);
}

Pointer master_thread(Pointer master_args){
    Master_args arguments = (Master_args) master_args;

    /**** Worker threads ****/

    pthread_t* workers;
    if((workers = malloc(sizeof(pthread_t) * arguments->numWorkerthreads)) == NULL){    // Allocate space for threads
        perror("worker thread malloc failure");  
        exit(EXIT_FAILURE); 
    }

    int error;
    for(int i = 0; i < arguments->numWorkerthreads; i++) {
        if((error = pthread_create((workers + i), NULL, worker_thread, (Pointer) arguments->worker_args))){   // Master creates the workers
            perror2("pthread creation failure", error);   
            exit(EXIT_FAILURE);
        } 
    }

    static struct sigaction act;        // Setting our handler so in 
    act.sa_handler = signal_handler;    // case of ctrl-c it will 
	sigemptyset(&act.sa_mask);          // terminate the program
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);

    socklen_t clientlen;
    int server_socket, new_socket;
    struct sockaddr_in server, client;
    struct sockaddr* serverptr = (struct sockaddr*) &server;
    struct sockaddr* clientptr = (struct sockaddr*) &client;

    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){  // Creating a socket (using AF_INET domain)
        perror("server socket creation failure"); 
        exit(EXIT_FAILURE); 
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(arguments->portnum);    // This is our port number

    if(bind(server_socket, serverptr, sizeof(server)) < 0){ // Bind socket to address
        perror("bind server socket failure");
        exit(EXIT_FAILURE);
    }

    if(listen(server_socket, PENDING_CONNECTIONS) < 0){ // Listen for connections
        perror("listen failure");
        exit(EXIT_FAILURE);
    }

    while(1){
        clientlen = sizeof(client);
        if((new_socket = accept(server_socket, clientptr, &clientlen)) < 0){
            if(errno == EINTR){ // If system call is ctrl-c terminate
                break;
            }
            perror("accept connection failure");
            exit(EXIT_FAILURE);
        }
        printf("Accepted connection\n");
        place_to_buffer(arguments->buffer, new_socket); // Master thread only places sockets file desciptors in the buffer
    }

    for(int i = 0; i < arguments->numWorkerthreads; i++){
        if((error = pthread_join(*(workers + i), NULL))){   // Wait all the worker threads 
            perror2("pthread join failure", error); 
            exit(EXIT_FAILURE); 
        }
    }
    
    mem_free(workers);

    close(server_socket);
    
    pthread_exit((Pointer) EXIT_SUCCESS); // Terminate
}