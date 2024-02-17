#include "poller.h"

struct voters{
    int party;      // How many parties there are
    int counter;    // How many people voted a party
    char** voted;   // What the voter voted
    char** voter;   // The name of the person that voted
    char** parties; // The party with their total votes 
};

// Initialize voters values
static void initialize_voters(Voters voters){
    voters->party = 0;
    voters->counter = 0;
}   

Voters create_voters_array(){
    Voters voters = (Voters) mem_allocate(sizeof(struct voters));
    voters->voted = (char**) mem_allocate(sizeof(char*) * INITIAL_SIZE);
    voters->voter = (char**) mem_allocate(sizeof(char*) * INITIAL_SIZE);
    voters->parties = (char**) mem_allocate(sizeof(char*) * INITIAL_SIZE);

    for(int i = 0; i < INITIAL_SIZE; i++){
        voters->voted[i] = (char*) mem_allocate(sizeof(char) * MAXSTRING);
        voters->voter[i] = (char*) mem_allocate(sizeof(char) * MAXSTRING);
        voters->parties[i] = (char*) mem_allocate(sizeof(char) * MAXSTRING);
    }

    initialize_voters(voters);
    return voters;
}

void destroy_voters_array(Voters voters){
    for(int i = 0; i < INITIAL_SIZE; i++){
        mem_free(voters->voted[i]);
        mem_free(voters->voter[i]);
        mem_free(voters->parties[i]);
    }
    mem_free(voters->parties);
    mem_free(voters->voted);
    mem_free(voters->voter);
    mem_free(voters);
}

bool check_if_already_voted(Voters voters, char* name){
    for(int i = 0; i < voters->counter; i++){
        if(strcmp(voters->voter[i], name) == 0){    // Check if the name matches and voter
            return true;                            // This voter doesnt have the right to vote again
        }
    }
    return false;
}

void insert_voter_name(Voters voters, char* name){
    strcpy(voters->voter[voters->counter], name);   // Insert in n position a new name that voted
}

void insert_voted_party(Voters voters, char* name){
    strcpy(voters->voted[voters->counter], name);   // Insert in n what a person voted
    increase_persons(voters);

    if(voters->party == 0){
        strcpy(voters->parties[voters->party], name);
        voters->party++;
    }else{
        bool unique = true;
        for(int i = 0; i < voters->party; i++){
            if(strcmp(voters->parties[i], name) == 0){
                unique = false;
                break;
            }
        }
        if(unique == true){
            strcpy(voters->parties[voters->party], name);
            voters->party++;
        }
    }
}

void increase_persons(Voters voters){
    voters->counter++;
}

void make_party_votes(Voters voters){
    int party_votes = 0;
    for(int i = 0; i < voters->party; i++){
        for(int j = 0; j < voters->counter; j++){
            if(strcmp(voters->parties[i], voters->voted[j]) == 0){
                party_votes++;
            }
        }
        char updated_party[MAXSTRING];
        sprintf(updated_party, "%s %d", voters->parties[i], party_votes);   // Creating the updated partie-votes string and
        strcpy(voters->parties[i], updated_party);                          // copying it into the array the string with party and votes
        party_votes = 0;
    }
}

void print_party_votes(Voters voters, FILE* stats_file){
    for(int i = 0; i < voters->party; i++){
        fprintf(stats_file, "%s\n", voters->parties[i]);  // Registre the party and votes to stats
    }
    fprintf(stats_file, "TOTAL %d\n", voters->counter);
}