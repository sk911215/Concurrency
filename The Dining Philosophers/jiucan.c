
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define NUM 5

#define THINKING 0
#define HUNGRY 1
#define EATING 2

int state[NUM];
int forks[NUM];
pthread_t p_thread[NUM];

pthread_mutex_t mutex[2] = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond[5] = PTHREAD_COND_INITIALIZER;

void *philosopher(void *num);
void get_forks(int phi, int l_fork, int r_fork);
void put_forks(int phi, int l_fork, int r_fork);
void get(int phi, int l_fork, int r_fork);

int main(int argc, char **argv)
{
    int i;
    for(i = 0; i < NUM; i++){
        forks[i] = 1;
    }
    for(i = 0; i < NUM; i++){
        pthread_create(&p_thread[i], NULL, philosopher, (void *)(intptr_t)i);
    }
    for(i = 0; i < NUM; i++){
        pthread_join(p_thread[i], NULL);
    }
    return EXIT_SUCCESS;
}

void *philosopher(void *num)
{
    int pos = (intptr_t)num;
    int left_fork, right_fork;
    int wait_think, wait_eat;
    
    state[pos] = THINKING;
    printf("Thinking... [%d]\n", pos);
    
    left_fork = pos;
    right_fork = (pos + 1) % NUM;
    
    while(true){
        //think()
        wait_think = rand() % 20 + 1;
        sleep(wait_think);
        get_forks(pos, left_fork, right_fork);
        if(state[pos] == EATING){
            printf("Eating... [%d]\n", pos);
            //eat()
            wait_eat = rand() % 8 + 2;
            sleep(wait_eat);
            put_forks(pos, left_fork, right_fork);
        }
    }
    pthread_exit(NULL);
}

void get_forks(int phi, int l_fork, int r_fork)
{
    int wait_eat;
    pthread_mutex_lock(&mutex[0]);
    state[phi] = HUNGRY;
    get(phi, l_fork, r_fork);
    if(state[phi] != EATING){
        printf("Waiting... [%d]\n", phi);
        while(true){
            if(state[(phi+1)%NUM] == EATING && state[(phi+4)%NUM] == EATING){
                pthread_cond_wait(&cond[l_fork], &mutex[0]);
                pthread_cond_wait(&cond[r_fork], &mutex[0]);
                get(phi, l_fork, r_fork);
            }
            else if(state[(phi+1)%NUM] == EATING){
                pthread_cond_wait(&cond[r_fork], &mutex[0]);
                get(phi, l_fork, r_fork);
            }
            else{
                pthread_cond_wait(&cond[l_fork], &mutex[0]);
                get(phi, l_fork, r_fork);
            }
            
            if(state[phi] == EATING){
                break;
            }
        }
    }
    pthread_mutex_unlock(&mutex[0]);
}

void put_forks(int phi, int l_fork, int r_fork)
{
    pthread_mutex_lock(&mutex[0]);
    pthread_mutex_lock(&mutex[1]);
    forks[l_fork] = 1;
    forks[r_fork] = 1;
    printf("Forks: %d-%d-%d-%d-%d\n", forks[0],forks[1],forks[2],forks[3],forks[4]);
    state[phi] = THINKING;
    printf("Thinking... [%d]\n", phi);
    pthread_mutex_unlock(&mutex[1]);
    pthread_mutex_unlock(&mutex[0]);
    pthread_cond_signal(&cond[l_fork]);
    pthread_cond_signal(&cond[r_fork]);
}

void get(int phi, int l_fork, int r_fork)
{
    if(state[(phi+1)%NUM] != EATING && state[(phi+4)%NUM] != EATING && state[phi] == HUNGRY){
        state[phi] = EATING;
        pthread_mutex_lock(&mutex[1]);
        forks[l_fork] = 0;
        forks[r_fork] = 0;
        printf("Forks: %d-%d-%d-%d-%d\n", forks[0],forks[1],forks[2],forks[3],forks[4]);
        pthread_mutex_unlock(&mutex[1]);
        pthread_cond_signal(&cond[l_fork]);
        pthread_cond_signal(&cond[r_fork]);
    }
}
