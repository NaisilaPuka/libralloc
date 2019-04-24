#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include "pthread.h"
#include "ralloc.h"

int handling_method;          // deadlock handling method

//CHANGE THE PARAMETERS FOR M AND N AS YOU WISH

#define M 10 // number of resource types
int exist[10] =  {12, 8, 10, 8, 10, 12, 8, 12, 8, 10}; // resources existing in the system

#define N 12 // number of processes - threads
pthread_t tids[N];            // ids of created threads

void *aprocess (void *p)
{
    int req[10];
    int k;
    int pid;
    
    pid =  *((int *)p);
    
    printf ("this is thread %d\n", pid);
    fflush (stdout);
    
    req[0] = 9;
    req[1] = 7;
    req[2] = 8;
    req[3] = 7;
    req[4] = 8;
    req[5] = 9;
    req[6] = 7;
    req[7] = 9;
    req[8] = 7;
    req[9] = 8;
    ralloc_maxdemand(pid, req); 
    
    for (k = 0; k < 2; ++k) {
        
        req[0] = 1;
        req[1] = 1;
        req[2] = 1;
        req[3] = 1;
        req[4] = 1;
        req[5] = 1;
        req[6] = 1;
        req[7] = 1;
        req[8] = 1;
        req[9] = 1;
        ralloc_request (pid, req);

        //sleep(2);

        ralloc_release(pid, req);

        //sleep(2);

        ralloc_request (pid, req);

        //sleep(2);

        ralloc_release(pid, req);
        
        //sleep(2);

        ralloc_request (pid, req);

        //sleep(2);

        ralloc_release(pid, req);
    }
    pthread_exit(NULL); 
}


int main(int argc, char **argv)
{

    int deadlocked[N]; // array indicating deadlocked processes
    int k;
    int i;
    int pids[N];
    
    for (k = 0; k < N; ++k)
        deadlocked[k] = -1; // initialize
    
    //CHANGE THIS TO THE DEADLOCK HANDLING METHOD YOU WANT TO TEST

    handling_method = DEADLOCK_DETECTION;
    ralloc_init (N, M, exist, handling_method);

    printf ("library initialized\n");
    fflush(stdout);
    
    //start time
    struct timeval  tv0;
    gettimeofday(&tv0, NULL);

    for (i = 0; i < N; ++i) {
        pids[i] = i;
        pthread_create (&(tids[i]), NULL, (void *) &aprocess, (void *)&(pids[i])); 
    }
    
    printf ("threads created = %d\n", N);
    fflush (stdout);
    
    //wait for all threads to finish
    for(int i = 0; i < N; i++)
    {
        pthread_join(tids[i],NULL);
    }

    //end time
    struct timeval  tv1;
    gettimeofday(&tv1, NULL);

     //get start and end in microseconds
    long long start = tv0.tv_sec*10000000 + tv0.tv_usec;
    long long end = tv1.tv_sec*10000000 + tv1.tv_usec;

    fflush (stdout);
    printf("Required time is %lld microseconds.\n", (end-start));

    //this is actually meaningless
    //since there is no deadlock at this point
    //however, we can still measure how much time the DETECTION algorithm takes
    //so, THIS IS ONLY FOR STATISTICAL REASONS

    if(handling_method == DEADLOCK_DETECTION)
    {
        struct timeval  tv2;
        gettimeofday(&tv2, NULL);
        ralloc_detection(deadlocked);
        struct timeval  tv3;
        gettimeofday(&tv3, NULL);
        start = tv2.tv_sec*10000000 + tv2.tv_usec;
        end = tv3.tv_sec*10000000 + tv3.tv_usec;
        fflush (stdout);
        printf("Required time for a single DETECTION is %lld microseconds.\n", (end-start));

    }
}