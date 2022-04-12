#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include "readerwriter.h"




typedef struct _rwlock_t {
    sem_t writelock;
    sem_t lock;
    int readers;
} rwlock_t;

void rwlock_init(rwlock_t *lock) {
    lock->readers = 0;
    sem_init(&lock->lock, 1, 100); 
    sem_init(&lock->writelock, 1,100); 
}

void rwlock_acquire_readlock(rwlock_t *lock) {
    sem_wait(&lock->lock);
    lock->readers++;
    if (lock->readers == 1)
	sem_wait(&lock->writelock);
    sem_post(&lock->lock);
}

void rwlock_release_readlock(rwlock_t *lock) {
    sem_wait(&lock->lock);
    lock->readers--;
    if (lock->readers == 0)
	sem_post(&lock->writelock);
    sem_post(&lock->lock);
}

void rwlock_acquire_writelock(rwlock_t *lock) {
    sem_wait(&lock->writelock);
}

void rwlock_release_writelock(rwlock_t *lock) {
    sem_post(&lock->writelock);
}

int read_loops;
int write_loops;
int counter = 0;

rwlock_t mutex;

void *readThread(void *arg) {
    int local = 0;
    
	rwlock_acquire_readlock(&mutex);
    printf("Create reader \n");
    int x=0, T;      
    T = rand()%10000;   
    for(int i = 0; i < T; i++)   
        for(int j = 0; j < T; j++)    
            x=i*j;  
	local = counter;
	rwlock_release_readlock(&mutex);
	
 
    printf("read done: \n");
    return NULL;
}

void *writeThread(void *arg) {
    
	rwlock_acquire_writelock(&mutex);
    printf("Create Write\n");   
    int x=0, T;      
    T = rand()%10000;   
    for(int i = 0; i < T; i++)   
        for(int j = 0; j < T; j++)    
            x=i*j;  

	counter++;
	rwlock_release_writelock(&mutex);

    printf("write done\n");
    return NULL;
}

int main()
{
    //variables
    //struct _rwlock_t *lock = malloc(sizeof(struct _rwlock_t));
    char rw;
    FILE *file;
    int error;

    //open inputfile
    file = fopen("scenarios.txt", "r");

    //initialize lock
   //rwlock_init(&mutex);
     rwlock_init(&mutex); 
    //for threads
    pthread_t thread;

    if (file)
    {
        //scan input file
        printf("Scenario Starts:\n");
        while (fscanf(file, "%c", &rw) != EOF)
        {

            if (rw == 'r')
            {

                //if read create a thread to run the readThread
                error = pthread_create(&thread, NULL, (void *)readThread, NULL);
                if (error != 0)
                {
                    printf("Can't create thread.\n");
                    return 1;
                }
            }

            else if (rw == 'w')
            {

                //if write create a thread to run the writeThread
                error = pthread_create(&thread, NULL, (void *)writeThread, NULL);
                if (error != 0)
                {
                    printf("Can't create thread.\n");
                    return 1;
                }
            }
        }
    }

    //close file
    fclose(file);

    //exit threads
    pthread_exit(NULL);

    return 0;
}