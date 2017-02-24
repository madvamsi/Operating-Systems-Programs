OBJS = process thread context_switch
CC = gcc

all: process thread context_switch

process:
	$(CC) process.c -o process

thread:
	$(CC) thread.c -lpthread -o thread

context_switch:
	$(CC) context_switch.c -o context_switch
	
clean:
	rm -rf $(OBJS)