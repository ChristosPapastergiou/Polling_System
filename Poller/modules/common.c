#include "poller.h"

Pointer mem_allocate(size_t size){
    Pointer array = calloc(1, size);    // calloc to initialize the values to 0
    return array;
}

void mem_free(Pointer pointer){
    free(pointer);
}

void clean_string(char* string){
    string[strcspn(string, "\n")] = 0;  // Remove new line
    string[strcspn(string, "\r")] = 0;  // Remove escape sequence
}

char* connect_strings(char* string1, char* string2){
    strcat(string1, " ");
    strcat(string1, string2);
    return string1;
}