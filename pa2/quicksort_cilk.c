/*
 * quicksort_cilk.c : A parallel quicksort implementation using cilkplus.
 *  
 *  Authors: Ameem Shaik and Zach Cross
 *  See README for details regarding compilation/dependencies.
 *
 *  Usage details:
 *      
 *      Uniformly random input array of size specified by argv[1].
 *      Number of cilk workers utilized optionally specified by argv[2].
 * 
 *  Implementation details (high level):
 *      
 *    Partition approach: SCAN-based partition using comparison flags.
 *    Pivot selection: always choose right (since input array is random).
 *    
 *  Optimizations (worth noting):
 * 
 *    Memory layout of comparison flags (interleaved) allows for one call
 *    to prefix sum rather than two per partition.
 *
 *    Degradation to sequential quicksort and sequential insertion sort
 *    based on input size of subproblem being a factor of "original"
 *    input size specified by argv[1].
 *
 */

/* Standard includes */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

/* Custom includes */
#include "quicksort_common.h"

// Array size at which to degrade to insertion sort.
#define SERIAL_INSERTION_NSIZE 32
#define SERIAL_PARTITION_N_FACTOR 0.7

typedef struct{
    int lte;
    int gt;
} lte_gt;

lte_gt *flags;
long *copyArray;    

// Cilk constant: # workers
int WORKERS,
    PROBLEM_SIZE;

double log2( double n )  
{  
    return log( n ) / log( 2 );  
}

/* Inclusive, in-place parallel prefix sum. 
    Takes an input X[1..n], n=2^k and produces
    an output S[1..n], a vector of prefix sums.
*/
void parallel_prefix_sum(lte_gt *S, int left, int n, int k) {

    int i, h;
    for(h = 1 ; h <= k ; h++) {
        cilk_for (i = 1; i <= (n >> h); i++) {
            S[left+i * (1 << h) - 1].lte += S[left+(1 << h) * i - (1 << (h-1)) - 1].lte;
            S[left+i * (1 << h) - 1].gt += S[left+(1 << h) * i - (1 << (h-1)) - 1].gt;
        }
    }

    for (h = k ; h >= 1; h--) {
        cilk_for(i = 2; i <= (n >> (h-1)); i++){
            if (i % 2) {
                S[left+i * (1 << (h-1)) -1].lte += S[left+i * (1 << (h-1)) - (1 << (h-1)) - 1].lte;
                S[left+i * (1 << (h-1)) -1].gt += S[left+i * (1 << (h-1)) - (1 << (h-1)) - 1].gt;
            }
        }
    }

}

void sequential_insertionSort(long *array, int left, int right) {

    int i, j, val;
    for(i = left; i <= right; i++) {
        val = array[i];
        j = i - 1;
        while( j >= 0 && array[j] > val) {
            array[j+1] = array[j];
            j--;
        }
        array[j+1] = val;
    }
}

int partition(long *array, int left, int right){
    // Compute n, k for helper prefix sum
    int n = (right - left + 1),
        k = (int) log2(n),
        i;

    // Get a random pivot
    // i = random_int(left, right+1);

        // Use right as pivot since we already randomized the array.
    long pivot = array[right];
    // Set flags in comparison flag arrays
    // Don't need eq anymore, just keep it in lt (except the pivot)
    cilk_for (i = left; i <= right; i++) {
        copyArray[i] = array[i];
        if (array[i] < pivot) {
            flags[i].lte = 1;
            flags[i].gt = 0;
        } else if(array[i]>pivot) {
            flags[i].lte = 0;
            flags[i].gt = 1;
        }
        else{
            if(i==right){
                flags[i].lte=0;
            }
            else {
                flags[i].lte=1;
            }
            flags[i].gt=0;
        }
    }
    parallel_prefix_sum(flags, left, n,k);
    int pivotIndex = left+flags[right].lte;
    //add the pivot
    array[pivotIndex] = pivot;
    // Now use these mappings to swap in parallel
    // Note, we don't look at i = right, since its the pivot
    cilk_for (i = left; i < right; i++){
        if(copyArray[i]<=pivot){
            array[left+flags[i].lte-1] = copyArray[i];
        }
        else if (copyArray[i]>pivot){
            array[pivotIndex+flags[i].gt] = copyArray[i];
        }
    }
    return pivotIndex;
}

void quicksort_recursive(long *array,int left,int right){

   // dbg_printf("quicksort(array, %d, %d)\n", left, right);

    if(left >= right) {
        return;
    }

    //First see if we've degraded to serial insertion sort.
    if (right - left + 1 < SERIAL_INSERTION_NSIZE) {
        return sequential_insertionSort(array, left, right);
    }
    else {
       // int  splitPoint = partition(array,left, right,copyArray);
        int splitPoint;
        if (right - left + 1 <= SERIAL_PARTITION_N_FACTOR * PROBLEM_SIZE) {
            splitPoint = sequential_partition(array,left, right);
        }
        else{
            splitPoint = partition(array,left, right);
        }
      //  dbg_printArray(array, left, right);
        cilk_spawn quicksort_recursive(array,left,splitPoint-1);
        quicksort_recursive(array,splitPoint+1,right);
    }
}

void quicksort(long *array, int size) {
    copyArray = (long *) malloc (sizeof(long) * size);
    flags = (lte_gt *) malloc (sizeof(lte_gt) * size);

    int i;
    cilk_for(i = 0 ; i < size ; i++) {
        copyArray[i] = 0;
        flags[i].lte = 0;
        flags[i].gt = 0;
    }

    quicksort_recursive(array, 0, size-1);
}

int main(int argc, char **argv) {

    double start ,stop;
    double time_elapsed;

    if(argc < 2){
        dbg_printf("Incorrect number of arguments, expected 1 argument\n");
        return EXIT_FAILURE;
    } else if (argc == 3) {
        WORKERS = (atoi(argv[2]) < __cilkrts_get_nworkers()) ? atoi(argv[2]): __cilkrts_get_nworkers();
    } else {
        WORKERS =  __cilkrts_get_nworkers(); 
    }

    PROBLEM_SIZE = atoi(argv[1]);
    long *array;
    array = malloc(PROBLEM_SIZE*sizeof(long));
    int i;

     __cilkrts_set_param("nworkers", argv[2]);
    dbg_printf("Using %d available workers.\n", WORKERS);

    // Initialize array with uniformly random values. Do in cilk_for to warm cache.
    srand(time(NULL));
    cilk_for(i = 0; i < PROBLEM_SIZE; i++){
        array[i] = random_int(0, PROBLEM_SIZE);
    }

    dbg_printf("Unsorted Array\n");
    dbg_printArray(array, 0, PROBLEM_SIZE-1);

    start = wctime();
    quicksort(array, PROBLEM_SIZE);
    stop = wctime();
    time_elapsed = (double) (stop - start);

    dbg_printf("Sorted Array\n");
    dbg_printArray(array, 0, PROBLEM_SIZE-1);
    printf("parallel, %d, %d, %f\n", PROBLEM_SIZE, WORKERS, time_elapsed);
    return 0;
}
