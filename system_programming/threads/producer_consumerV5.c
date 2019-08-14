/********************************
* 	 Author  : Daniel Maizel	*
*	 Date    : 27/05/2019		*
*	 Reviewer: Eldad        	*
*								*
*********************************/
#include <pthread.h>/* threads */
#include <stdlib.h>/* malloc */ 
#include <unistd.h> /* sleep */
#include <stdio.h>/* printf */
#include <assert.h>/* assert */
#include <semaphore.h>  
#include <string.h>

#define BUFFER_SIZE 256
#define NUM_CONSUMERS 5
#define NUM_PRODUCERS 1

char buffer[BUFFER_SIZE];
pthread_cond_t consume_add;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 
sem_t max_counter;
int g_cons_counter = 0;

void Produce()
{		
	printf("Producing...\n");
	strcpy(buffer,"Holla, My name is Danil!");
}

void Consume()
{
	printf("Consuming...\n");
	printf("data: %s\n", buffer);
	++g_cons_counter;
}

void *Producer(void *data)
{
	(void)data;

	while(1)
	{
		int i = 0;

		for(i = 0; i < NUM_CONSUMERS; ++i) /*Making sure each consumer*/
		{                                  /*Has Reached the condition*/ 
			sem_wait(&max_counter);        /*variable.                */
		}

		Produce();

		pthread_mutex_lock(&lock);
		pthread_cond_broadcast(&consume_add);	
		pthread_mutex_unlock(&lock);	
	}

	return NULL;
}

void *Consumer(void *data)
{
	(void)data;

	while(1)
	{
		pthread_mutex_lock(&lock);

		sem_post(&max_counter);
		if(0 == pthread_cond_wait(&consume_add, &lock))
		{
			Consume();
		}
		
		pthread_mutex_unlock(&lock);
	}

	return NULL;
}

int main()
{
	int ret = 0;
	int i = 0;
	pthread_t *consumers_id = (pthread_t *)malloc(sizeof(pthread_t) * NUM_CONSUMERS);
	pthread_t *producers_id = (pthread_t *)malloc(sizeof(pthread_t) * NUM_PRODUCERS);

	sem_init(&max_counter, 0, 0);


	for(i = 0; i < NUM_PRODUCERS; ++i)
	{
		ret = pthread_create(&producers_id[i], NULL, Producer, NULL);
		if(ret != 0) 
		{
			perror("error: \n");
	    }
	}

	for(i = 0; i < NUM_CONSUMERS; ++i)
	{
		ret = pthread_create(&consumers_id[i], NULL, Consumer, NULL);
		if(ret != 0) 
		{
			perror("error: \n");
	    }
	}

    for(i = 0; i < NUM_PRODUCERS; ++i)
	{
		ret = pthread_join(producers_id[i], NULL);
		if(ret != 0) 
		{
			perror("error: \n");
	    }
	}

    for(i = 0; i < NUM_CONSUMERS; ++i)
	{
		ret = pthread_join(consumers_id[i], NULL);
		if(ret != 0) 
		{
			perror("error: \n");
	    }
	}

    pthread_mutex_destroy(&lock);
    sem_destroy(&max_counter); 

  	return 0;
}