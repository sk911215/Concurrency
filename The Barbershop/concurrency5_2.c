/* CS544
 * Concurrency 5 --- Problem 2
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
#include <signal.h>	

#define MAX_CUSTOMER 10
#define MAX_CHAIR 5
#define SLEEPING 0
#define WORKING 1

int state = SLEEPING;
int isFullShop = 0;
int noCustomer = 0;
int isChair = 0; 
int num_chair = 0;
int noMoreCustomer = MAX_CUSTOMER;

pthread_cond_t free_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t wake_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cut_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t next_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t work_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t work_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t do_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t state_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t state1_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t shop_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_t c_pthread[MAX_CUSTOMER];
pthread_t b_pthread;

void *customer (void*);
void *barber (void*);

int main (int argc, char** argv)
{
	int i;
	pthread_create(&b_pthread, NULL, barber, NULL);
	for(i = 0; i < MAX_CUSTOMER; i++){
		pthread_create(&c_pthread[i], NULL, customer, (void *)(intptr_t)i);
	}
	for(i = 0; i < MAX_CUSTOMER; i++){
		pthread_join(c_pthread[i], NULL);
	}
	noCustomer = 1;
	printf("rrr");
	pthread_join(b_pthread, NULL);
	
	return EXIT_SUCCESS;
}
void *barber (void* a)
{
	while(noCustomer == 0){
		if(state == SLEEPING){
			pthread_cond_wait(&wake_cond, &state_mutex);
			state = WORKING;
			printf("Barber: wake up.\n");
			pthread_mutex_unlock(&state_mutex);
			pthread_cond_signal(&work_cond);
		}
		pthread_cond_wait(&cut_cond, &work_mutex);
		//cut_hair();
		printf("Barber: cut hair..\n");
		//cut_hair();
		pthread_mutex_unlock(&work_mutex);		
		pthread_cond_signal(&free_cond);
		if(state == SLEEPING && noMoreCustomer == 0){
                        break;
                }
	}
	pthread_exit(NULL);
}
void *customer (void* a)
{
	pthread_mutex_lock(&state1_mutex);
	if(state == SLEEPING){
		pthread_mutex_lock(&state_mutex);
		printf("Customer[%d]: waking barber up\n", a);
		pthread_cond_signal(&wake_cond);
		pthread_cond_wait(&work_cond, &state_mutex);
		pthread_mutex_unlock(&state_mutex);
	}
	pthread_mutex_unlock(&state1_mutex);
	pthread_mutex_lock(&shop_mutex);
	if(isFullShop == 1){
		noMoreCustomer--;
		printf("Customer[%d]: exit full shop.\n", a);
		pthread_mutex_unlock(&shop_mutex);
		pthread_exit(NULL);
	}
	if(num_chair < MAX_CHAIR){
		num_chair++;
		noMoreCustomer--;
		printf("Customer[%d]: enter shop.\n", a);
		if(num_chair == MAX_CHAIR){
			isFullShop = 1;
		}
	}
	pthread_mutex_unlock(&shop_mutex);
	pthread_mutex_lock(&do_mutex);
	if(num_chair > 1){
		pthread_cond_wait(&next_cond, &do_mutex);
	}
	//get_hair_cut();
	sleep(1);	
	printf("Customer[%d]: get hair cut.\n", a);
	sleep(2);
	pthread_cond_signal(&cut_cond);
	pthread_cond_wait(&free_cond, &do_mutex);
	//get_hair_cut();
	printf("Customer[%d]: done hair cut and exit.\n", a);
	pthread_mutex_lock(&shop_mutex);
	num_chair--;
	if(isFullShop == 1){
		isFullShop = 0;
	}
	if(num_chair == 0){
		state = SLEEPING;
		printf("Barber: sleeping.\n");
		if(noMoreCustomer == 0){
			printf("No more customer.\n");
			raise(SIGINT);
		}
	}
	pthread_mutex_unlock(&shop_mutex);
	pthread_cond_signal(&next_cond);
	pthread_mutex_unlock(&do_mutex);
	pthread_exit(NULL);
}
