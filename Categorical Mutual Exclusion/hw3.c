/* CS544
 * Concurrency 3
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

#define NUM 5
struct linklist {
	struct linklist *next;
	int num;
};

struct linklist *head;

struct linklist *list_end(void){
	struct linklist *cur = head;
	while (cur->next != NULL)
	{
		cur =cur ->next;
	}
	return cur;
}

int s_num = 0;
int inserting = 0;
int deleting = 0;

pthread_t s_thread[NUM];
pthread_t i_thread[10];
pthread_t d_thread[NUM];

pthread_mutex_t db_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t search_mutex = PTHREAD_MUTEX_INITIALIZER;	
pthread_mutex_t insert_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t delete_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t search_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t insert_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t delete_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t delete_to_search_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t delete_to_insert_cond = PTHREAD_COND_INITIALIZER;

void *searcher(void* a);
void *inserter(void* a);
void *deleter(void* a); 

void free_lists(void);
void display_lists(void);

int main(int argc, char **argv)
{
	head = (struct linklist*)malloc(sizeof(struct linklist));
        head->next =NULL;
        head->num = 0;
	
	int i;
	for(i = 0; i < NUM; i++){
		pthread_create(&s_thread[i], NULL, searcher, (void *)(intptr_t)i+1);
	} 
	for(i = 0; i < 10; i++){
		pthread_create(&i_thread[i], NULL, inserter, (void *)(intptr_t)i+1);
	}
	for(i = 0; i < NUM; i++){
		pthread_create(&d_thread[i], NULL, deleter, (void *)(intptr_t)i+1);
	}
	for(i = 0; i < NUM; i++){
		pthread_join(s_thread[i], NULL);
	}
	
	for(i = 0; i < 10; i++){
                pthread_join(i_thread[i], NULL);
        }
	for(i = 0; i < NUM; i++){
                pthread_join(d_thread[i], NULL);
        }
	printf("The list after operations is: ");
	display_lists();
	free_lists();
	printf("Finished...\n");
	return EXIT_SUCCESS;
}
 
void free_lists(void){
	struct linklist *cur;
	cur = head;
	while(cur != NULL){
		head = head -> next;
		free(cur);
		cur = head;
	}
}

void display_lists(void){
	struct linklist *cur;
	cur = head;
	while(cur != NULL){
		printf("%d -> ", cur -> num);
		cur = cur -> next;
	}
	printf("NULL\n");	 
}

void *searcher(void* a){
	int t_num;
	t_num = rand() % 5;
	sleep(t_num);
	if(deleting == 1){
		printf("***searcher[%d] waiting...\n", a);
		pthread_cond_wait(&delete_to_search_cond, &db_mutex);
	}
	else{
		pthread_mutex_lock(&db_mutex);
	}
	s_num++;
	printf("***searcher[%d] started\n", a);
	pthread_mutex_lock(&search_mutex);
	display_lists();
	pthread_mutex_unlock(&search_mutex);
	pthread_mutex_unlock(&db_mutex);

	s_num--;
	if(s_num == 0 && deleting == 1){
		pthread_cond_signal(&search_cond);
	}
	pthread_mutex_lock(&search_mutex);
	printf("***searcher[%d] finished\n", a);
	pthread_mutex_unlock(&search_mutex);
	pthread_exit(NULL);
}

void *inserter(void* a){
	int t_num;
	int nodeNum = 0;
	struct linklist *node;
	struct linklist *end;
	t_num = rand() % 3;
	if(deleting == 1){
		pthread_cond_wait(&delete_to_insert_cond, &db_mutex);
	}
	else{
		pthread_mutex_lock(&db_mutex);
	}
	sleep(t_num);
	pthread_mutex_lock(&insert_mutex);
	printf("===inserter[%d] started\n", a);
	inserting = 1;
	
	nodeNum = rand() % 10;
	node = (struct linklist*)malloc(sizeof(struct linklist));
	node->next = NULL;
	node->num = nodeNum;
	end = list_end();
	end->next = node;
	printf("===insert num is [%d]\n", nodeNum);
	
	inserting = 0;
	pthread_mutex_unlock(&insert_mutex);
	pthread_mutex_unlock(&db_mutex);
	pthread_cond_signal(&insert_cond);
	
	printf("===inserter[%d] finished\n", a);
	pthread_exit(NULL);
}

void *deleter(void* a){
	int del = 0;
	int t_num;
	int delNum;
	struct linklist *cur;
	delNum = rand() % 10;
	t_num = rand() % 7 + 5;
	sleep(t_num);
	pthread_mutex_lock(&delete_mutex);
	printf("---deleter[%d] started\n",a);
	deleting = 1;
	if(s_num != 0){
		printf("---deleter[%d] waiting for searchers finished\n", a);
		pthread_cond_wait(&search_cond, &search_mutex);
	}
	if(inserting == 1){
		printf("---deleter[%d] waiting for inserter finished\n", a);
		pthread_cond_wait(&insert_cond, &insert_mutex);
	}
	pthread_mutex_lock(&db_mutex);
	
	cur = head;
	while(cur->next != NULL) {
		if(cur->next->num == delNum) {
			struct linklist *tmp = cur->next;
			printf("---deleted num is [%d]\n", delNum);
			cur->next = cur->next->next;
			free(tmp);
			del = 1;
			break;
		}
		cur = cur->next;
	}
	if(del == 0){
		printf("---delete num [%d] could'n find\n", delNum);
	}
	pthread_mutex_unlock(&db_mutex);
	deleting == 0;
	pthread_cond_signal(&delete_to_search_cond);
	pthread_cond_signal(&delete_to_insert_cond);
	pthread_cond_signal(&delete_cond);
	pthread_mutex_unlock(&delete_mutex);
	pthread_mutex_unlock(&insert_mutex);
	pthread_mutex_unlock(&search_mutex);
	printf("---deleter[%d] finished\n", a);
	pthread_exit(NULL);
}

