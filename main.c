#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>




typedef struct _rwlock_t {
    sem_t writelock;
    sem_t readlock;
    sem_t resource;
    int readers;
    int writers;
} rwlock_t;

void rwlock_init(rwlock_t *lock) {
    lock->readers = 0;
    lock->writers = 0;
    sem_init(&lock->readlock, 0, 1); 
    sem_init(&lock->writelock, 0, 1); 
    sem_init(&lock->resource,0, 1); 
}

void rwlock_acquire_readlock(rwlock_t *lock) {
    sem_wait(&lock->readlock);
    
    
    if (lock->readers == 0 || lock->writers > 0){
	    sem_post(&lock->readlock);
        sem_wait(&lock->resource); 
        sem_wait(&lock->readlock);
        //printf("%d Wait writelock " ,lock->readers);
    }
    lock->readers++;
    sem_post(&lock->readlock);
    printf("Create reader \n");
}

void rwlock_release_readlock(rwlock_t *lock) {
    sem_wait(&lock->readlock);
    lock->readers--;
    //printf("%d Read count -- " ,lock->readers);
    if (lock->readers == 0 ){
	    sem_post(&lock->resource);
        //printf("%d post writelock " ,lock->readers);
    }
    sem_post(&lock->readlock);
    printf("read done: \n");
}

void rwlock_acquire_writelock(rwlock_t *lock) {
    sem_wait(&lock->writelock);  
    lock->writers++;
    sem_post(&lock->writelock); 
    sem_wait(&lock->resource);
    printf("Create Writer\n");   
}

void rwlock_release_writelock(rwlock_t *lock) {
    sem_wait(&lock->writelock);
     lock->writers--;
    sem_post(&lock->writelock);
    sem_post(&lock->resource);
     printf("write done\n");
}




void *readThread(void *arg) {
    
    
	rwlock_acquire_readlock(arg);
    //printf("Create reader \n");
    int x=0, T;      
    T = rand()%10000;   
    for(int i = 0; i < T; i++)   
        for(int j = 0; j < T; j++)    
            x=i*j;  
	
	rwlock_release_readlock(arg);
	
 
    //printf("read done: \n");
    return NULL;
}

void *writeThread(void *arg) {
    
	rwlock_acquire_writelock(arg);  
    //printf("Create Writer\n");   
    int x=0, T;      
    T = rand()%10000;   
    for(int i = 0; i < T; i++)   
        for(int j = 0; j < T; j++)    
            x=i*j;  

	
	rwlock_release_writelock(arg);

    //printf("write done\n");
    return NULL;
}

int main()
{
    //variables
    struct _rwlock_t *lock = malloc(sizeof(struct _rwlock_t));
    char rw;
    FILE *file;
    int error;

    //open inputfile
    file = fopen("scenarios.txt", "r");

    //initialize lock
   rwlock_init(lock);
     
    //for threads
    pthread_t thread;

    if (file)
    {
        //scan input file
        printf("Scenario Starts:\n");
        while (fscanf(file, "%c", &rw) != EOF)
        {
            //printf("%c\n",rw);
            if (rw == 'r')
            {

                //if read create a thread to run the readThread
                error = pthread_create(&thread, NULL, (void *)readThread, (void *)&lock);
                if (error != 0)
                {
                    printf("Can't create thread.\n");
                    return 1;
                }
            }

            else if (rw == 'w')
            {

                //if write create a thread to run the writeThread
                error = pthread_create(&thread, NULL, (void *)writeThread, (void *)&lock);
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