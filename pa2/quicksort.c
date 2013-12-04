/*
 * quicksort.c : A sequential quicksort implementation for comparison.
 *  
 *  Authors: Ameem Shaik and Zach Cross
 *  See README for details regarding compilation/dependencies.
 *
 *  Usage details:
 *      
 *      Uniformly random input array of size specified by argv[1].
 * 
 *  Implementation details (high level):
 *      
 *    Partition approach: typical efficient in-place swapping.
 *    Pivot selection: always choose right (since input array is random).
 */

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

/* Custom includes */
#include "quicksort_common.h"

int main(int argc, char **argv)
{

    int problem_size;
    double start, stop;
    double time_elapsed;

    if(argc!=2){
        printf("Incorrect number of arguments, expected 1 argument\n");
        return EXIT_FAILURE;
    }
    problem_size = atoi(argv[1]);
    long *array;
    array = malloc(problem_size*sizeof(long));
    int i;
    srand(time(NULL));

    // Initialize uniformly distributed input array
    for(i = 0; i < problem_size; i++){
        array[i] = random_int(0, problem_size);
    }

    #ifdef PRINTMODE
    printf("Unsorted Array\n");
    printArray(array,0, problem_size-1);
    #endif

    int left = 0;
    int right = problem_size-1;
    start = wctime();
    sequential_quicksort(array, left, right);
    stop = wctime();
    time_elapsed = (double) (stop - start);

    #ifdef PRINTMODE
    printf("Sorted Array\n");
    printArray(array,0, problem_size-1);
    #endif
    printf("sequential, %d, %d, %f\n", problem_size, 1, time_elapsed);
    return 0;
}
