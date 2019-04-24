
#include <stdio.h>
#include <stdbool.h> 
#include <pthread.h>
#include <stdlib.h>
#include "ralloc.h"

int p_cnt = 0; //number of processes
int r_cnt = 0; //number of distinct resources
int * r_existing; //existing resources in the system
int deadlock = 0; //deadlock handling algorithm
int * avail; //available resources in the system
int ** alloc; //allocated resources to processes in the system
int ** max; //maximum demand of each process for each resource
int ** need; //maximum request of each process for each resource
int ** request; //current request of waiting processes
pthread_mutex_t lock; //lock for accessing library data
pthread_cond_t cond; //condition variable to wait when resources cannot be allocated
int init = 0; //this shows whether the library has been initialized

// this includes printfs for debugging; you will delete them in final version.

int ralloc_init(int p_count, int r_count, int r_exist[], int d_handling)
{
    //Check that the number of processes is appropriate
    if(p_count > MAX_PROCESSES || p_count < 1)
    {
        printf("The number of processes should be between 1 and %d!\n", MAX_PROCESSES);
        return -1;
    }

    //Check that the number of resources is appropriate
    if(r_count > MAX_RESOURCE_TYPES || r_count < 1)
    {        
        printf("The number of resources should be between 1 and %d!\n", MAX_RESOURCE_TYPES);
        return -1;
    }

    //Check that the deadlock handling method is appropriate
    if(d_handling > 3 || d_handling < 1)
    {
        printf("Deadlock handling should be either 1, 2 or 3! \n");
        return -1;
    }

    //initialize necessary variables for resource allocation
    p_cnt = p_count;

    r_cnt = r_count;

    r_existing = malloc(r_cnt * sizeof(int));
    for(int i = 0; i < r_cnt; i++)
    {
        r_existing[i] = r_exist[i];
    }

    deadlock = d_handling;

    avail = malloc(r_cnt * sizeof(int));
    for(int i = 0; i < r_cnt; i++)
    {
        avail[i] = r_exist[i];
    }

    alloc = malloc(p_cnt * sizeof(int *));
    for(int i = 0; i < p_cnt; i++)
    {
        alloc[i] = malloc(r_cnt * sizeof(int));
        for(int j = 0; j < r_cnt; j++)
            alloc[i][j] = 0;
    }

    if(deadlock == 3)
    {
        max = malloc(p_cnt * sizeof(int *));
        for(int i = 0; i < p_cnt; i++)
        {
            max[i] = malloc(r_cnt * sizeof(int));
            for(int j = 0; j < r_cnt; j++)
                max[i][j] = 0;
        }

        need = malloc(p_cnt * sizeof(int *));
        for(int i = 0; i < p_cnt; i++)
        {
            need[i] = malloc(r_cnt * sizeof(int));
            for(int j = 0; j < r_cnt; j++)
                need[i][j] = 0;
        }
    }

    pthread_mutex_init(&lock, NULL);

    pthread_cond_init(&cond, NULL);

    //initialize request matrix if deadlock handling is detection
    if(deadlock == 2)
    {
        request = malloc(p_cnt * sizeof(int *));
        for(int i = 0; i < p_cnt; i++)
        {
            request[i] = malloc(r_cnt * sizeof(int));
            for(int j = 0; j < r_cnt; j++)
                request[i][j] = 0;
        }
    }

    init = 1;
    
    return (0);
}

int ralloc_maxdemand(int pid, int r_max[])
{
    //check pid bounds
    if(pid < 0 || pid >= p_cnt)
    {
        printf("Process pid is not eligible!\n");
        return -1;
    }

    if(deadlock == 3)
    {
        pthread_mutex_lock(&lock);
        //check r_max bounds
        for(int i = 0; i < r_cnt; i++)
        {
            if(r_max[i] > r_existing[i])
            {
                printf("This process is requesting more resources than existing!\n");
                pthread_mutex_unlock(&lock);
                return -1;
            }

            else
            {
                max[pid][i] = r_max[i];
                need[pid][i] = r_max[i];
            }
        }

        pthread_mutex_unlock(&lock);
        return (0);
    }
    return -1;
}

int ralloc_request (int pid, int demand[])
{
    //check pid bounds
    if(pid < 0 || pid >= p_cnt)
    {
        printf("Process pid is not eligible!\n");
        return -1;
    }

    pthread_mutex_lock(&lock);

    //check if process is breaking the rules of maximum demand
    //in case of deadlock avoidance
    if(deadlock == 3)
    {
        for(int i = 0; i < r_cnt; i++)
        {
            if(demand[i] > need[pid][i])
            {
                printf("This process is requesting more resources than its maximum demand!\n");
                pthread_mutex_unlock(&lock);
                return -1;
            }
        }
    }

    int i;
    //check if process is requesting more than existing in the system
    for(i = 0; i < r_cnt; i++)
    {
        if((demand[i] + alloc[pid][i]) > r_existing[i])
        {
            printf("Process is requesting more resources than existing in the system!\n");
            pthread_mutex_unlock(&lock);
            return -1;
        }
    }

    //declare necessary variables for deadlock avoidance
    int hadProblem = 0;
    int* work;
    int* finish;
    int done;
    int thisOk = 1;

    while(1)
    {
        //check if request <= available
        for(i = 0; i < r_cnt; i++)
        {
            if(demand[i] > avail[i])
            {
                hadProblem = 1;

                //save in request matrix if we are using detection deadlock handling
                if(deadlock == 2)
                {
                    for(int j = 0; j < r_cnt; j++)
                    {
                        request[pid][j] = demand[j];
                    }
                }
                
                pthread_cond_wait(&cond, &lock);
                
                //remove from request matrix if we are using detection deadlock handling
                if(deadlock == 2)
                {
                    for(int j = 0; j < r_cnt; j++)
                    {
                        request[pid][j] = 0;
                    }
                }
                break;
            }
        }

        //if request was > available at least once, check again
        if(hadProblem)
        {
            hadProblem = 0;
            continue;
        }

        //if no deadlock avoidance algorithm, just give the resources
        if(deadlock == 1 || deadlock == 2)
        {
            for(i = 0; i < r_cnt; i++)
            {
                avail[i] = avail[i] - demand[i];
                alloc[pid][i] = alloc[pid][i] + demand[i];
            }
            pthread_mutex_unlock(&lock);
            break;
        }

        //we are in deadlock avoidance state
        //therefore we need to do deadlock avoidance algorithm
        //to check if the state is going to be safe
        else
        {
            //firstly allocate, and then check the new state
            for(i = 0; i < r_cnt; i++)
            {
                avail[i] = avail[i] - demand[i];
                alloc[pid][i] = alloc[pid][i] + demand[i];
                need[pid][i] = need[pid][i] - demand[i];
            }

            //initiate variables necessary for deadlock avoidance algorithm
            work = malloc(r_cnt * sizeof(int));
            finish = malloc(p_cnt * sizeof(int));

            //initialize work to initially the available vector
            for(i = 0; i < r_cnt; i++)
                work[i] = avail[i];

            //initialize all finish[i] to false for all processes
            for(i = 0; i < p_cnt; i++)
                finish[i] = 0;

            //keep a variable to check if finish[i] is true for all processes
            done = 0;

            //keep a variable to check if a process is not finished but can be finished
            //because its need is less than current work
            thisOk = 1;

            //deadlock avoidance algorithm
            while(!done)
            {
                 //find a process which hasn't finished but its need is less than work
                for(i = 0; i < p_cnt; i++)
                {
                    //find a process whose finish[i] is false
                    if(!finish[i])
                    {
                        //check if need < work for that process
                        for(int j = 0; j < r_cnt; j++)
                        {
                            if(need[i][j] > work[j])
                            {
                                thisOk = 0;
                                break;
                            }
                        }
                        //if need<work, this will be OK
                        if(thisOk)
                            break;
                    }
                }

                thisOk = 1;

                //no process unfinished has need < work
                if(i == p_cnt)
                    break;

                //add the allocated resources of process to work
                for(int j = 0; j < r_cnt; j++)
                {
                    work[j] = work[j] + alloc[i][j];
                }

                //that process is now finished!
                finish[i] = 1;

                //lets check if all processes are finished
                done = 1;
                for(int j = 0; j < p_cnt; j++)
                {
                    if(!finish[j])
                    {
                        done = 0;
                        break;
                    }
                }
            }

            //if all are done, the state is SAFE!
            if(done)
            {
                pthread_mutex_unlock(&lock);
                free(work);
                free(finish);
                break;
            }

            //if all are not done, the state is UNSAFE! Wait in condition variable
            else
            {
                //undo resource allocation
                for(i = 0; i < r_cnt; i++)
                {
                    avail[i] = avail[i] + demand[i];
                    alloc[pid][i] = alloc[pid][i] - demand[i];
                    need[pid][i] = need[pid][i] + demand[i];
                }
                free(work);
                free(finish);
                pthread_cond_wait(&cond, &lock);
            }
        }
    }
    return(0);
}

int ralloc_release (int pid, int demand[])
{
    //check pid bounds
    if(pid < 0 || pid >= p_cnt)
    {
        printf("Process pid is not eligible!\n");
        return -1;
    }

    pthread_mutex_lock(&lock);

    //check if accidentally, more than existing are being released!
    for(int i = 0; i < r_cnt; i++)
    {
        if(avail[i] + demand[i] > r_existing[i])
        {
            printf("You didn't get those resources from us!\n");
            pthread_mutex_unlock(&lock);
            return -1;
        }
    }

    //deallocate indicated resource instances for that project 
    for(int i = 0; i < r_cnt; i++)
    {
        avail[i] = avail[i] + demand[i];
        if(deadlock == 3)
        {
            need[pid][i] = need[pid][i] + demand[i];
        }
        alloc[pid][i] = alloc[pid][i] - demand[i];
    }

    //release other processes which might get resources allocated now that more
    //resources are available
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&lock);
    return (0); 
}

int ralloc_detection(int procarray[])
{
    if(deadlock == 2)
    {
        pthread_mutex_lock(&lock);
        int* work;
        int* finish;
        int i;
        int done = 0;
        int thisOk = 1;
        int count = 0;

        //declare variables necessary for deadlock avoidance algorithm
        work = malloc(r_cnt * sizeof(int));
        finish = malloc(p_cnt * sizeof(int));

        //initialize work to initially the available vector
        for(i = 0; i < r_cnt; i++)
            work[i] = avail[i];

        //initialize all finish[i] to false for all processes
        for(i = 0; i < p_cnt; i++)
        {
            finish[i] = 0;
        }

        while(!done)
        {
            //find a process which hasn't finished but its need is less than work
            for(i = 0; i < p_cnt; i++)
            {
                //find a process whose finish[i] is false
                if(!finish[i])
                {
                    //check if request < work for that process
                    for(int j = 0; j < r_cnt; j++)
                    {
                        if(request[i][j] > work[j])
                        {
                            thisOk = 0;
                            break;
                        }
                    }
                    //if need<work, this will be OK
                    if(thisOk)
                        break;
                }
            }

            thisOk = 1;
            
            //no process unfinished has request < work
            if(i == p_cnt)
                break;

            //add the allocated resources of process to work
            for(int j = 0; j < r_cnt; j++)
            {
                work[j] = work[j] + alloc[i][j];
            }

            //that process is now finished!
            finish[i] = 1;

            //lets check if all processes are finished
            done = 1;
            for(int j = 0; j < p_cnt; j++)
            {
                if(!finish[j])
                {
                    done = 0;
                    break;
                }
            }
        }

        //if all are done, NO DEADLOCK!
        if(done)
        {
            free(work);
            free(finish);
            pthread_mutex_unlock(&lock);
            return 0;
        }

        //if all are not done, there is DEADLOCK!
        else
        {
            for(i = 0; i < p_cnt; i++)
            {
                if(!finish[i])
                {
                    count++;
                    procarray[i] = 1;
                }
            }
            free(work);
            free(finish);
            pthread_mutex_unlock(&lock);
            return count;
        }
    }
    else
        return (0);
}

int ralloc_end()
{
    if(init == 1)
    {
        pthread_mutex_lock(&lock);

        free(r_existing);
        r_existing = NULL;

        free(avail);
        avail = NULL;

        for(int i = 0; i < r_cnt; i++)
        {
            free(alloc[i]);
            alloc[i] = NULL;
        }
        free(alloc);
        alloc = NULL;

        if(deadlock == 3)
        {
            for(int i = 0; i < r_cnt; i++)
            {
                free(max[i]);
                max[i] = NULL;
            }
            free(max);
            max = NULL;

            for(int i = 0; i < r_cnt; i++)
            {
                free(need[i]);
                need[i] = NULL;
            }
            free(need);
            need = NULL;
        }

        else if(deadlock == 2)
        {
            for(int i = 0; i < p_cnt; i++)
            {
                free(request[i]);
                request[i] = NULL;
            }
            free(request);
            request = NULL;
        }
        
        init = 0;
        p_cnt = 0;
        r_cnt = 0;
        deadlock = 0;

        pthread_cond_destroy(&cond);
        pthread_mutex_destroy(&lock);
    }
    return (0); 
}
