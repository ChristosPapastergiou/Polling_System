POLLER_MODULES = Poller/modules
POLLER_INCLUDE = Poller/include
SWAYER_MODULES = PollSwayer/modules
SWAYER_INCLUDE = PollSwayer/include

CC = gcc

POLLER_DEPS = $(POLLER_INCLUDE)/poller.h
POLLER_OBJS = poller.o buffer.o thread.o voters.o common.o
SWAYER_DEPS = $(SWAYER_INCLUDE)/pollSwayer.h
SWAYER_OBJS = pollSwayer.o guideThread.o server.o common.o 

POLLER_EXEC = poller
SWAYER_EXEC = pollSwayer

PORT = 5000
POLLER_ARGS = $(PORT) 10 5 pollLog.txt pollStats.txt
SWAYER_ARGS = localhost $(PORT) inputFile.txt

CFLAGS = -Wall -Werror -g 

all: poller pollSwayer

$(POLLER_EXEC): $(POLLER_OBJS)
	$(CC) $(POLLER_OBJS) -o $(POLLER_EXEC) -lpthread

poller.o: $(POLLER_MODULES)/poller.c $(POLLER_DEPS)
	$(CC) $(CFLAGS) -c $(POLLER_MODULES)/poller.c -I$(POLLER_INCLUDE)

common.o: $(POLLER_MODULES)/common.c $(POLLER_DEPS)
	$(CC) $(CFLAGS) -c $(POLLER_MODULES)/common.c -I$(POLLER_INCLUDE)

buffer.o: $(POLLER_MODULES)/buffer.c $(POLLER_DEPS)
	$(CC) $(CFLAGS) -c $(POLLER_MODULES)/buffer.c -I$(POLLER_INCLUDE)

thread.o: $(POLLER_MODULES)/thread.c $(POLLER_DEPS)
	$(CC) $(CFLAGS) -c $(POLLER_MODULES)/thread.c -I$(POLLER_INCLUDE)

voters.o: $(POLLER_MODULES)/voters.c $(POLLER_DEPS)
	$(CC) $(CFLAGS) -c $(POLLER_MODULES)/voters.c -I$(POLLER_INCLUDE)

$(SWAYER_EXEC): $(SWAYER_OBJS)
	$(CC) $(SWAYER_OBJS) -o $(SWAYER_EXEC) -lpthread

pollSwayer.o: $(SWAYER_MODULES)/pollSwayer.c $(SWAYER_DEPS)
	$(CC) -c $(SWAYER_MODULES)/pollSwayer.c -I$(SWAYER_INCLUDE)

guideThread.o: $(SWAYER_MODULES)/guideThread.c $(SWAYER_DEPS)
	$(CC) -c $(SWAYER_MODULES)/guideThread.c -I$(SWAYER_INCLUDE)

server.o: $(SWAYER_MODULES)/server.c $(SWAYER_DEPS)
	$(CC) -c $(SWAYER_MODULES)/server.c -I$(SWAYER_INCLUDE)

clean:
	rm -f $(POLLER_OBJS) $(POLLER_EXEC) $(SWAYER_OBJS) $(SWAYER_EXEC)

valgrindPoller: $(POLLER_EXEC)
	valgrind --leak-check=full --show-leak-kinds=all ./$(POLLER_EXEC) $(POLLER_ARGS)

valgrindSwayer: $(SWAYER_EXEC)
	valgrind --leak-check=full --show-leak-kinds=all ./$(SWAYER_EXEC) $(SWAYER_ARGS)

# --leak-check=full --show-leak-kinds=all