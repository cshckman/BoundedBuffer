#include <stdlib.h> 
#include <stdio.h>
#include "buffer.h"
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// creates the buffer
buffer_item buffer[BUFFER_SIZE];
int out = 0;
int in = 0;

//creates the semaphores
sem_t isempty;
sem_t isfull;
pthread_mutex_t mutex;

/* 	
	This inserts the item into the buffer and returns an error
	message if it finds that it is adding to the buffer in a location 
	that has something in it denoted with any number greater than -1
*/
int insert_item(buffer_item item) {
	pthread_mutex_lock(&mutex);// locks the access to the buffer

	if(buffer[in] == -1){
		buffer[in] = item;
		in = (in+1)%BUFFER_SIZE;
		pthread_mutex_unlock(&mutex);
		return 0;
	} else {
		pthread_mutex_unlock(&mutex);
		return -1;
	}
}

/*
	This removes the item from the buffer and puts that item
	inside the variable item. If has error checking that stops 
	it from removing something that hasn't been added yet
	denoted with a -1 	
*/
int remove_item(buffer_item *item){
	pthread_mutex_lock(&mutex); // locks the access to the buffer
	if(buffer[out] != -1){
		*item = buffer[out];
		buffer[out] = -1;
		out = (out+1)%BUFFER_SIZE;
		pthread_mutex_unlock(&mutex);
		return 0;
	} else {
		pthread_mutex_unlock(&mutex);
		return -1;
	}
}

void *producer(void *param){
	buffer_item item;

	while (1){
		//sleeps and creates a random number
		usleep(rand()%50*100000);
		item = rand();

		//stops other producers from producing when the buffer is full
		sem_wait(&isempty);
		if (insert_item(item))
			fprintf(stderr, "Buffer is full \n");
		else
			printf("producer produced %d\n", item);
		//allows the consumer to consume because there is now something in the buffer
		sem_post(&isfull);
	}
}

void *consumer(void *param){
	buffer_item item;

	while(1){
		//sleeps.
		usleep(rand()%50*100000);
		//This stops other consumers from consuming when the buffer is empty
		sem_wait(&isfull);
		if(remove_item(&item))
			fprintf(stderr, "Buffer is empty \n");
		else
			printf("consumer consumed %d\n" , item);
		//allows the producer to produce because there is now an empty spot in the buffer.
		sem_post(&isempty);
	}
}

int main(int argc, char *argv[]) {
	// handle inputs
	if (argc != 4){
		printf("Sorry but you do not have the proper arguments");
	} else{
		int sleeptime = atoi(argv[1]);
		int producerthreads = atoi(argv[2]);
		int consumerthreads = atoi(argv[3]);

		// Initializes all idicies of the buffer to -1
		for (int i = 0; i < BUFFER_SIZE; ++i){
			buffer[i] = -1;
		}
		pthread_mutex_init(&mutex,NULL);
		sem_init(&isempty, 0, BUFFER_SIZE);
		sem_init(&isfull, 0, 0);

		// Create producer threads
		pthread_t prod[producerthreads];
		for (int i = 0; i < producerthreads; ++i){
			pthread_create(&prod[i], NULL, producer, NULL);
		}
		// Create consumer threads
		pthread_t cons[consumerthreads];
		for (int i = 0; i < consumerthreads; ++i){
			pthread_create(&cons[i], NULL, consumer, NULL);
		}

		// Sleeps
		sleep(sleeptime);
		// Kills threads and ends.
		for (int i = 0; i < producerthreads; ++i){
			pthread_cancel(prod[i]);
		}
		for (int i = 0; i < consumerthreads; ++i){
			pthread_cancel(cons[i]);
		}
	}
	return 0;
}
