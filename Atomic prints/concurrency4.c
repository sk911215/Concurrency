/* CS544
 * Concurrency 4
 * Group 36
 * Kai Shi
 * Wen-Yin Wang
 * Xu Zheng
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

pthread_cond_t print_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t next_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_t pthread[2];

void *printF (void*);
void *printS (void*);

int main (int argc, char** argv)
{
	pthread_create(&pthread[0], NULL, printF, NULL);
	pthread_create(&pthread[1], NULL, printS, NULL);
	
	pthread_join(pthread[0], NULL);
	pthread_join(pthread[1], NULL);

	return EXIT_SUCCESS;
}

void *printF (void* a)
{
	int i = 0;	
	pthread_mutex_lock(&mutex);
	while(i < 20){
		printf("hello ");
		pthread_mutex_unlock(&mutex);
		pthread_cond_signal(&print_cond);
		pthread_cond_wait(&next_cond, &mutex);
		i++;
	} 
	pthread_exit(NULL);
}

void *printS (void* a)
{
	int i = 0;
	while(i < 20){
		pthread_cond_wait(&print_cond, &mutex);
		printf("world!\n");
		pthread_mutex_unlock(&mutex);
		pthread_cond_signal(&next_cond);
		i++;
	}
	pthread_exit(NULL);
}
