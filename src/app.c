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
#include <errno.h>

int handling_method;          // deadlock handling method

#define M 3                   // number of resource types
int exist[3] =  {12, 8, 10};  // resources existing in the system

#define N 20                   // number of processes - threads
pthread_t tids[N];            // ids of created threads

void *aprocess (void *p)
{
    int req[3];
    int k;
    int pid;
    
    pid =  *((int *)p);
    
    req[0] = 3;
    req[1] = 3;
    req[2] = 3;
    ralloc_maxdemand(pid, req); 
    
    for (k = 0; k < 8; ++k)
    {
        req[0] = 1;
        req[1] = 1;
        req[2] = 1;
        if(handling_method == DEADLOCK_DETECTION)
        {
            ralloc_request (pid, req);
            fflush(stdout);
            sleep(1);
            ralloc_release (pid, req);
            fflush(stdout);
            sleep(1);
            ralloc_request (pid, req);
            fflush(stdout);
            sleep(1);
            ralloc_release (pid, req);
            fflush(stdout);
            sleep(1);
            ralloc_request (pid, req);
            fflush(stdout);
            sleep(1);
            ralloc_release (pid, req);
            fflush(stdout);
            sleep(1);
            ralloc_request (pid, req);
            fflush(stdout);
            sleep(1);
            ralloc_release (pid, req);
            fflush(stdout);
            sleep(1);
            ralloc_request (pid, req);
            fflush(stdout);
            sleep(1);
            ralloc_release (pid, req);
            fflush(stdout);
            sleep(1);
            ralloc_request (pid, req);
            fflush(stdout);
            sleep(1);
            ralloc_release (pid, req);
            fflush(stdout);
            sleep(1);
            ralloc_request (pid, req);
            fflush(stdout);
            sleep(1);
            ralloc_release (pid, req);
            fflush(stdout);
            sleep(1);
            ralloc_request (pid, req);
            fflush(stdout);
            sleep(1);
            ralloc_release (pid, req);
            fflush(stdout);
            sleep(1);
        }
        else
        {
            ralloc_request (pid, req);
            ralloc_release (pid, req);
            ralloc_request (pid, req);
            ralloc_release (pid, req);
            ralloc_request (pid, req);
            fflush(stdout);
            sleep(1);
            ralloc_release (pid, req);
            ralloc_request (pid, req);
            ralloc_release (pid, req);
            ralloc_request (pid, req);
            ralloc_release (pid, req);
            ralloc_request (pid, req);
            ralloc_release (pid, req);
        }
    }
    pthread_exit(NULL); 
}


int main(int argc, char **argv)
{
    printf ("HELLO!\n");
    fflush(stdout);
    printf ("Welcome to your favorite Resource Allocation Library!\n");
    fflush(stdout);
    printf("\nNote: If there is Segmentation Fault, please check README.txt on how to invoke this application!\n\n");
    fflush(stdout);

    int dn; // number of deadlocked processes
    int deadlocked[N]; // array indicating deadlocked processes
    int k;
    int i;
    int pids[N];
    int count = 1;
    
    for (k = 0; k < N; ++k)
        deadlocked[k] = -1; // initialize

    if(atoi(argv[1]) == 2)
    {
        printf ("LET'S CHECK DEADLOCK DETECTION!\n");
        fflush(stdout);

        printf ("Library initialized with DETECTION...\n");
        fflush(stdout);

        handling_method = DEADLOCK_DETECTION;
        ralloc_init (N, M, exist, handling_method);

        printf ("\n");
        printf ("Threads created = %d\n", N);
        fflush (stdout);

        for (i = 0; i < N; ++i)
        {
            pids[i] = i;
            pthread_create (&(tids[i]), NULL, (void *) &aprocess, (void *)&(pids[i])); 
        }

        printf ("Running detection algorithm 20 times every second...\n");
        fflush (stdout);

        while (count <= 21)
        {
            fflush(stdout);
            sleep (1); // detection period
            if (handling_method == DEADLOCK_DETECTION)
            {
                printf("\nRunning deadlock detection algorithm...#%d\n", count);
                count++;
                dn = ralloc_detection(deadlocked);
                if (dn > 0) 
                {
                    printf ("THERE ARE %d DEADLOCKED PROCESSES AT THIS INSTANT!\n", dn);
                }
                else
                {
                    printf ("No deadlock at this instant!\n");
                }
            }
        }

        printf ("\n");
        fflush(stdout);
        printf ("Ending library ...\n");
        return 0;
    }
    else if(atoi(argv[1]) == 3)
    {

        printf ("LET'S CHECK DEADLOCK AVOIDANCE!\n");
        fflush(stdout);
        printf ("Library initialized with AVOIDANCE...\n");
        fflush(stdout);

        handling_method = DEADLOCK_AVOIDANCE;
        ralloc_init (N, M, exist, handling_method);

        printf ("\n");
        fflush(stdout);
        printf ("Threads created = %d\n", N);
        fflush (stdout);
        printf ("Scenario allows all threads to finish after a certain amount of time!\n");
        fflush (stdout);
        printf ("Please wait...(~30 seconds)\n");
        fflush(stdout);

        for (i = 0; i < N; ++i)
        {
            pids[i] = i;
            pthread_create (&(tids[i]), NULL, (void *) &aprocess, (void *)&(pids[i])); 
        }

        //start time
        struct timeval  tv0;
        gettimeofday(&tv0, NULL);

        //wait for all threads to finish
        for(int i = 0; i < N; i++)
        {
            pthread_join(tids[i], NULL);
        }

        //end time
        struct timeval  tv1;
        gettimeofday(&tv1, NULL);

        //get start and end in microseconds
        long long elapsedTime = (tv1.tv_sec - tv0.tv_sec) * 1000.0;      // sec to ms
        elapsedTime += (tv1.tv_usec - tv0.tv_usec) / 1000.0;

        printf("Required time for all threads to finish with DEADLOCK_AVOIDANCE is %lld milliseconds.\n", elapsedTime);
        fflush (stdout);

        printf ("\n");
        fflush(stdout);
        printf ("Ending library ...\n");
        fflush(stdout);

        ralloc_end();

        printf ("\n");
        fflush(stdout);
    }
    else if(atoi(argv[1]) == 1)
    {
        printf ("LET'S CHECK DEADLOCK NOTHING!\n");
        fflush(stdout);
        printf ("Library initialized with NOTHING...\n");
        fflush(stdout);

        handling_method = DEADLOCK_NOTHING;
        ralloc_init (N, M, exist, handling_method);

        printf ("\n");
        fflush(stdout);
        printf ("Threads created = %d\n", N);
        fflush (stdout);

        printf ("This might take some time (~20 seconds)!\n");
        fflush(stdout);

        for (i = 0; i < N; ++i) {
            pids[i] = i;
            pthread_create (&(tids[i]), NULL, (void *) &aprocess, (void *)&(pids[i])); 
        }

        //start time
        struct timeval tv0;
        gettimeofday(&tv0, NULL);

        //wait for all threads to finish
        for(int i = 0; i < N; i++)
        {
            pthread_join(tids[i], NULL);
        }

        //end time
        struct timeval tv1;
        gettimeofday(&tv1, NULL);
        
         //get start and end in microseconds
        long long elapsedTime = (tv1.tv_sec - tv0.tv_sec) * 1000.0;      // sec to ms
        elapsedTime += (tv1.tv_usec - tv0.tv_usec) / 1000.0;

        printf("Required time for all threads to finish with DEADLOCK_NOTHING is %lld milliseconds.\n", elapsedTime);
        fflush (stdout);

        ralloc_end();

        printf ("\n");
        fflush(stdout);
    }
    
    printf("END OF APPLICATION! BYE!\n");
    fflush(stdout);
    return 0;
}