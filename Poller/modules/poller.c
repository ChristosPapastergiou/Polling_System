#include "poller.h"

int main(int argc, char** argv){
    /**** Argument check ****/

    if(argc != 6){
        if(argc < 6){
            printf("Not enough arguments. Try again.\n");
            exit(EXIT_FAILURE);
        }
        printf("Too many arguments. Try again.\n");
        exit(EXIT_FAILURE);
    }
    
    /**** Data ****/

    int portnum = atoi(argv[1]);
    int numWorkerthreads = atoi(argv[2]);
    int bufferSize = atoi(argv[3]);
    char* poll_log = argv[4];
    char* poll_stats = argv[5];

    /**** Files ****/

    FILE* log_file;
    if((log_file = fopen(poll_log, "w")) == NULL){
        perror("fopen failure");
        exit(EXIT_FAILURE);
    }

    FILE* stats_file;
    if((stats_file = fopen(poll_stats, "w")) == NULL){
        perror("fopen failure");
        exit(EXIT_FAILURE);
    }

    /**** Buffer & Threads ****/

    Voters voters = create_voters_array();
    Buffer buffer = create_buffer(bufferSize);  // Creating the shared buffer

    int error;
    pthread_t master;
    Master_args master_args = create_master_args(buffer, voters, portnum, numWorkerthreads, log_file);  // Creating master arguments
    if((error = pthread_create(&master, NULL, master_thread, (Pointer) master_args))){  // Creating master thread
        perror2("pthread creation failure", error);
        exit(EXIT_FAILURE);
    }

    sigset_t sigset;                            // Setting our handler to ignore 
    sigemptyset(&sigset);                       // ctrl-c signal because main
    sigaddset(&sigset, SIGINT);                 // must go on until master
    pthread_sigmask(SIG_BLOCK, &sigset, NULL);  // gets this signal

    if((error = pthread_join(master, NULL))){   // Wait the master thread
        perror2("pthread join failure", error);
        exit(EXIT_FAILURE);
    }

    make_party_votes(voters);
    print_party_votes(voters, stats_file);

    fclose(log_file);
    fclose(stats_file);

    destroy_master_args(master_args);
    destroy_voters_array(voters);
    destroy_buffer(buffer);

    exit(EXIT_SUCCESS);
}