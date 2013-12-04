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

/* Custom includes */
#include "quicksort_common.h"

int main(int argc, char **argv)
{

    // Problem size, specified via argv[1].
    int problem_size;

    // Timing-related variables.
    double start, stop, time_elapsed;

    // Validate run-time arguments.
    if(argc!=2){
        printf("Incorrect number of arguments, expected 1 argument\n");
        return EXIT_FAILURE;
    }

    // Prepare input array: allocate memory.
    problem_size = atoi(argv[1]);
    long* array = (long*) malloc(problem_size * sizeof(long));
    int i;

    // Seed random number generator used for input array generation.
    srand(time(NULL));

    // Initialize uniformly distributed input array using an unbiased function.
    for(i = 0; i < problem_size; i++){
        array[i] = random_int(0, problem_size);
    }

    // If we are in PRINTMODE, print the array before sorting.
    #ifdef PRINTMODE
    printf("Unsorted Array\n");
    printArray(array,0, problem_size-1);
    #endif

    // Start the clock, run quicksort, stop the clock.
    start = wctime();
    sequential_quicksort(array, 0, problem_size - 1);
    stop = wctime();
    time_elapsed = (double) (stop - start);

    // If we are in PRINTMODE, print the array after sorting.
    #ifdef PRINTMODE
    printf("Sorted Array\n");
    printArray(array,0, problem_size-1);
    #endif

    // Always print a CSV-ready row for data summary:
    // (algorithm type, problem size, number of workers, time elapsed (sec))
    printf("sequential, %d, %d, %f\n", problem_size, 1, time_elapsed);
    return 0;
}
