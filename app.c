#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"
#include "ralloc.h"

int handling_method;          // deadlock handling method

#define M 3                   // number of resource types
int exist[3] =  {12, 8, 10};  // resources existing in the system

#define N 5                   // number of processes - threads
pthread_t tids[N];            // ids of created threads

void *aprocess (void *p)
{
    int req[3];
    int k;
    int pid;
    
    pid =  *((int *)p);
    
    printf ("this is thread %d\n", pid);
    fflush (stdout);
    
    req[0] = 2;
    req[1] = 2;
    req[2] = 2;
    ralloc_maxdemand(pid, req); 
    
    for (k = 0; k < 10; ++k) {
        
        req[0] = 1;
        req[1] = 1;
        req[2] = 1;
        ralloc_request (pid, req);
	
        // do something with the resources
	
        ralloc_release (pid, req);

        // call request and release as many times as you wish with
        // different parameters
    }
    pthread_exit(NULL); 
}


int main(int argc, char **argv)
{
    int dn; // number of deadlocked processes
    int deadlocked[N]; // array indicating deadlocked processes
    int k;
    int i;
    int pids[N];
    
    for (k = 0; k < N; ++k)
        deadlocked[k] = -1; // initialize
    
    handling_method = DEADLOCK_DETECTION;
    ralloc_init (N, M, exist, handling_method);

    printf ("library initialized\n");
    fflush(stdout);
    
    for (i = 0; i < N; ++i) {
        pids[i] = i;
        pthread_create (&(tids[i]), NULL, (void *) &aprocess,
                        (void *)&(pids[i])); 
    }
    
    printf ("threads created = %d\n", N);
    fflush (stdout);
    
    while (1) {
        sleep (10); // detection period
        if (handling_method == DEADLOCK_DETECTION) {
            dn = ralloc_detection(deadlocked);
            if (dn > 0) {
                printf ("there are deadlocked processes\n");
            }
        }
        // write code for:
        // if all treads terminated, call ralloc_end and exit
    }    
}
