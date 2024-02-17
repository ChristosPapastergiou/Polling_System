#include "pollSwayer.h"

int main(int argc, char** argv){
    /**** Argument check ****/

    if(argc != 4){
        if(argc < 4){
            printf("Not enough arguments. Try again.\n");
            exit(EXIT_FAILURE);
        }
        printf("Too many arguments. Try again.\n");
        exit(EXIT_FAILURE);
    }

    /**** Data ****/

    char* serverName = argv[1];
    int portNum = atoi(argv[2]);
    char* inputFile = argv[3];

    /**** File ****/

    FILE* file;
    if((file = fopen(inputFile, "r")) == NULL){
        perror("file opening failure");
        exit(EXIT_FAILURE);
    }

    int total_lines = 0;
    char line[MAXSTRING];
    while(fgets(line, sizeof(line), file) != NULL){ // Counting documents lines because
        total_lines++;                              // the number of lines in the txt equals
    }                                               // the total amount of threads to be created 
    rewind(file);

    /**** Threads & Server ****/

    int error;
    Server server = create_server(portNum, serverName);
    pthread_t* guide = (pthread_t*) mem_allocate(sizeof(pthread_t) * total_lines);
    Guide_args* guide_args = (Guide_args*) mem_allocate(sizeof(Guide_args) * total_lines);

    char* temp_line = (char*) mem_allocate(sizeof(char) * MAXSTRING);   // Allocating memory for temp
    char* temp_name = (char*) mem_allocate(sizeof(char) * MAXSTRING);   // data to be able to pass the 
    char* temp_party = (char*) mem_allocate(sizeof(char) * MAXSTRING);  // as the wanted arguments in guide_args

    int i = 0;
    while(fgets(temp_line, sizeof(char) * MAXSTRING, file) != NULL){
        clean_string(temp_line);

        char* temp = strtok(temp_line, STRTOK_DELIMITER);
        if(temp == NULL){
            printf("Need name.\n");
            continue;
        }
        strcpy(temp_name, temp);    // The first string is the name

        temp = strtok(NULL, STRTOK_DELIMITER);
        if(temp == NULL){
            printf("Need surname.\n");
            continue;
        }
        temp_name = connect_strings(temp_name, temp);   // Next to name the surname from 2nd string

        temp = strtok(NULL, STRTOK_DELIMITER);
        if(temp == NULL){
            printf("Need vote for party.\n");
            continue;
        }
        strcpy(temp_party, temp);   // And the votes party

        guide_args[i] = create_guide_args(server, temp_name, temp_party);   // Every thread has it's arguments
        if((error = pthread_create((guide + i), NULL, guide_thread, (Pointer) guide_args[i]))){    // Create the thread
            perror2("pthread creation failure", error);
            exit(EXIT_FAILURE);
        }

        i++;
    }

    for(int i = 0; i < total_lines; i++){
        if((error = pthread_join(*(guide + i), NULL))){ // Wait for all the threads
            perror2("pthread join failure", error);
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < total_lines; i++){
        destroy_guide_args(guide_args[i]);
    }

    mem_free(temp_line);
    mem_free(temp_name);
    mem_free(temp_party);
    mem_free(guide_args);
    mem_free(guide);
    
    destroy_server(server);
    fclose(file);

    exit(EXIT_SUCCESS);
}