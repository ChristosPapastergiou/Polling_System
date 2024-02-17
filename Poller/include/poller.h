#ifndef POLLER
#define POLLER

#define _GNU_SOURCE // Just for sigaction struct (without -> incomplete type)

/**** Includes ****/

#include <errno.h>
#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/**** Defines ****/

#define SPACE " "
#define VOTED "ALREADY VOTED\n"
#define VOTE "SEND VOTE PLEASE\n"
#define NAME "SEND NAME PLEASE\n"
#define PARTY "VOTE for Party %s RECORDED\n"

#define MAXSTRING 256
#define INITIAL_SIZE 1000
#define PENDING_CONNECTIONS 5
#define perror2(s,e) fprintf(stderr, "%s: %s\n", s, strerror(e))

/**** Global ****/

// Flag signaling on 1 if ctrl-c is pressed 
extern int sig_flag;

// Conditional variable for master that buffer is full cant put anything else
extern pthread_cond_t not_full;

// Conditional variable for worker that buffer is empty cant obtain anything
extern pthread_cond_t not_empty;

// Mutex for master-workers to save-take socket's file desc
extern pthread_mutex_t buf_mutex;

/**** Typedefs ****/

typedef void* Pointer;
typedef struct voters* Voters;
typedef struct buffer* Buffer;
typedef struct worker_args* Worker_args;
typedef struct master_args* Master_args;

/**** buffer.c ****/

// Creates connection buffer and returns it
Buffer create_buffer(int bufferSize);

// Destroys the connection buffer
void destroy_buffer(Buffer buffer);

// Placing to buffer a socket descriptor so when a worker wants 
// a socket to obtain it and master thread can keep placing other sockets
void place_to_buffer(Buffer buffer, int new_socket);

// Obtaining from buffer a socket descriptor that worker 
// thread needs so master thread can keep placing another sockets
int obtain_from_buffer(Buffer buffer);

/**** common.c ****/

// Allocates memory for Pointer with size size
Pointer mem_allocate(size_t size);

// Frees memory of the Pointer 
void mem_free(Pointer pointer);

// Deleting new line and escape sequence from a string
void clean_string(char* name);

// Connecting two string with a space between them
char* connect_strings(char* string1, char* string2);

/**** thread.c ****/

// Creates master thread arguments and returns it
Master_args create_master_args(Buffer buffer, Voters voters, int portnum, int numWorkerthreads, FILE* log_file);

// Destroying master arguments
void destroy_master_args(Master_args master_args);

// Worker thread function
Pointer worker_thread(Pointer worker_args);

// Master thread function
Pointer master_thread(Pointer master_args);

/**** voters.c ****/

// Creates an array that holds voters with what they voted
Voters create_voters_array();

// Destroying voters array
void destroy_voters_array(Voters voters);

// Checks if voter with name = name already voted
bool check_if_already_voted(Voters voters, char* name);

// Insert a voter with name = name
void insert_voter_name(Voters voters, char* name);

// Inserting what a pesron voted
void insert_voted_party(Voters voters, char* name);

// Increases the number of persons that voted a party
void increase_persons(Voters voters);

// Makes every party stats
void make_party_votes(Voters voters);

// Prints to poll_stats file the parties and their total votes
void print_party_votes(Voters voters, FILE* stats_file);

#endif