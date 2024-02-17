#include "poller.h"

struct buffer{
    int end;    
    int size;   // Buffer's size
    int start;
    int count;  // Counter to know how many socket fds buffer has
    int* data;  // Array of chars to save file descriptors of the sockets
};

// Initialize buffer's values
static void initialize_buffer(Buffer buffer, int bufferSize){
    buffer->end = -1;
    buffer->count = 0;
    buffer->start = 0;
    buffer->size = bufferSize;
}

Buffer create_buffer(int bufferSize){
    Buffer buffer = (Buffer) mem_allocate(sizeof(struct buffer));   // Allocate space for the buffer
    buffer->data = (int*) mem_allocate(sizeof(int) * bufferSize);   // And the buffer data
    initialize_buffer(buffer, bufferSize);
    return buffer;
} 

void destroy_buffer(Buffer buffer){
    mem_free(buffer->data);
    mem_free(buffer);
}

void place_to_buffer(Buffer buffer, int new_socket){
    pthread_mutex_lock(&buf_mutex); // Locking buffer's mutex cause master thread wants to place a file desc of a socket

    while(buffer->count == buffer->size){               
        printf(">>Found Buffer Full\n");            // Wait on a conditional variable, we dont want to do busy waiting
        pthread_cond_wait(&not_full , &buf_mutex);  // And you are free to go whenever a worker found the file desc of the socket
    }
    buffer->end = (buffer->end + 1) % buffer->size;
    buffer->data[buffer->end] = new_socket;
    buffer->count++;

    pthread_mutex_unlock(&buf_mutex);   // Unlock the mutex
    pthread_cond_signal(&not_empty);    // And signal that a worker can continue it's job
}

int obtain_from_buffer(Buffer buffer){
    int socket_fd = 0;

    pthread_mutex_lock(&buf_mutex); // Locking buffer's mutex cause a worker wants to go find file desk of the socket

    while(buffer->count == 0){    
        printf(">>Found Buffer Empty\n");                  
        pthread_cond_wait(&not_empty, &buf_mutex);  // Wait on a conditional variable, we dont want to do busy waiting
        if(sig_flag == 1){                          // It will be signaled when master thread is done so there will be
            pthread_mutex_unlock(&buf_mutex);       // things to do, but when master gets ctrl-c return to finish
            return -1;                                  
        }
    }
    socket_fd = buffer->data[buffer->start];
    buffer->start = (buffer->start + 1) % buffer->size;
    buffer->count--;

    pthread_mutex_unlock(&buf_mutex);   // Unlock the mutex
    pthread_cond_signal(&not_full);     // And signal that a worker can continue it's job

    return socket_fd;   // The file descriptor of the socket we want 
}