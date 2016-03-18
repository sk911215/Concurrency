/* Class#: CS544
 * Project#: 1 
 * Group#: 36 
 * Group members: Kai Shi (shik), Xu Zheng (zhengxu), Wen-Yin Wang (wangweny)
 * Deadline: 04/13/15
 */

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdbool.h>
#include<pthread.h>

#define MAX_WORKERS 2

struct item{
	int num;
	int wait_time;
};

struct item buffer[32];
int buffer_count = 0;

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

void *producer (void* a);
void *consumer (void* a);

int main (int argc, char** argv){
	pthread_t p_thread[MAX_WORKERS];
	pthread_t c_thread[MAX_WORKERS];
	int i;

	for(i = 0; i < MAX_WORKERS; i++){
		pthread_create(&p_thread[i], NULL, producer, NULL);
		pthread_create(&c_thread[i], NULL, consumer, NULL);
	}
	for(i = 0; i < MAX_WORKERS; i++){
		pthread_join(p_thread[i], NULL);
		pthread_join(c_thread[i], NULL);
	}
	return EXIT_SUCCESS;
}

void* producer (void* a){
	int i = 0;
	while(i < 20) {
		int p_wait, item_num, c_wait;

		if (buffer_count >= 32){
                	printf("Buffer full: Blocked for consuming...\n");
			while(buffer_count >= 32){
			}
		}
		
		__asm__(
			"movl   $0, %eax\n\t"
			"call   rand\n\t"
			"movl   %eax, -12(%rbp)\n\t"
		);
		p_wait = 3 + p_wait % 6;
		sleep(p_wait);
		
		pthread_mutex_lock(&mutex1);

		__asm__(
			"movl   $0, %eax\n\t"
			"call   rand\n\t"
			"movl   %eax, -8(%rbp)\n\t"
		);
		item_num = 1 + item_num % 10;
		buffer[buffer_count].num = item_num;
		
		__asm__(
			"movl   $0, %eax\n\t"
			"call   rand\n\t"
			"movl   %eax, -4(%rbp)\n\t"
		);
		c_wait = 2 + c_wait % 8;
		buffer[buffer_count].wait_time = c_wait;
		
		printf("Produce a new item successfully.\n");
		buffer_count++;
		printf("Now have %d items.\n", buffer_count);
		
		pthread_mutex_unlock(&mutex1);
		i++;
	}
        pthread_exit(NULL);
}

void* consumer (void* a){
	int i = 0;
	int isfirstloop = 0;
	while(i < 20){
		if (buffer_count <= 0){
			if(isfirstloop == 0){
				printf("Buffer empty: Blocked for producing...\n");
				isfirstloop = 1;
			}
			while(true){
				if(buffer_count > 0){
					break;
 				}
			}
		}
		pthread_mutex_lock(&mutex1);
		if(buffer_count-1 >= 0){
			buffer_count--;
			sleep(buffer[buffer_count].wait_time);
			printf("Consume %d.\n", buffer[buffer_count].num);
			isfirstloop = 0;
			i++;
		}
		pthread_mutex_unlock(&mutex1);	
	}
        pthread_exit(NULL);
}
