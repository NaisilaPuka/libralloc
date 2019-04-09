
#include <stdio.h>
#include <pthread.h>
#include "ralloc.h"




// this includes printfs for debugging; you will delete them in final version.

int ralloc_init(int p_count, int r_count, int r_exist[], int d_handling)
{
    printf ("ralloc_init called\n");

        
    return (0); 
}

int ralloc_maxdemand(int pid, int r_max[]){
    
    printf ("ralloc_maxdemand called by %d\n", pid);

    return (0); 
}
int ralloc_request (int pid, int demand[]) {
    printf ("ralloc_request called\n");
    return(0); 
}
int ralloc_release (int pid, int demand[]) {
    printf ("ralloc_release called\n"); 
    return (0); 
}

int ralloc_detection(int procarray[]) {
    printf ("ralloc_detection called\n");
    return (0); 
}

int ralloc_end() {
    printf ("ralloc_end called\n");
    return (0); 
}
