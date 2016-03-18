/* CS544
 * Concurrency 5 --- Problem 1	
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

#define MAX_THREAD 10

pthread_cond_t full_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t num_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_t pthread[MAX_THREAD];

int num_thread = 0;
int isFull = 0;

void *accessR (void*);

int main (int argc, char** argv)
{
	int i;
	for(i = 0; i < MAX_THREAD; i++){
		pthread_create(&pthread[i], NULL, accessR, (void *)(intptr_t)i);
	}	
	
	for(i = 0; i < MAX_THREAD; i++){
		pthread_join(pthread[i], NULL);
	}

	return EXIT_SUCCESS;
}

void *accessR (void* a)
{
	pthread_mutex_lock(&num_mutex);
	if(num_thread < 3){
		if(isFull == 0){
			num_thread++;
		}
	}
	else{
		isFull = 1;
	}
	if(isFull == 1){
		printf("Waiting for enter [%d]\n", a);
		pthread_cond_wait(&full_cond, &num_mutex);
		num_thread++;
	}
	pthread_mutex_unlock(&num_mutex);
	printf("Starting... [%d]\n", a);
	pthread_mutex_lock(&data_mutex);
	sleep(2);
	pthread_mutex_unlock(&data_mutex);
	printf("Ending... [%d]\n", a);
	
	pthread_mutex_lock(&num_mutex);
	num_thread--;
	//printf("Current thread:[%d]\n", num_thread);
	pthread_mutex_unlock(&num_mutex);
	if(num_thread == 0 && isFull == 1){
		isFull == 0;
		pthread_cond_signal(&full_cond);
		pthread_cond_signal(&full_cond);
		pthread_cond_signal(&full_cond);
	} 
	
	pthread_exit(NULL);
}

