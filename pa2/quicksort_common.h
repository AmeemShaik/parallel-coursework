/*
 * quicksort_common.c : Code common to quicksort.c and quicksort_cilk.c
 *  
 *  Authors: Ameem Shaik and Zach Cross
 *  See README for details regarding compilation/dependencies.
 *
 */

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

/************************************************
 *  HELPERS.
 ***********************************************/ 

/* Compute the log base 2. Apparently math.h log2 is not portable. */
double log2( double n )  
{  
    return log( n ) / log( 2 );  
}

/* Generated a random integer in [low, high). */
int random_int (unsigned int low, unsigned int high)
{
    int random = rand();
    if (RAND_MAX == random) {
        return random_int(low, high);
    }

    int range = high - low,
        remain = RAND_MAX % range,
        slot = RAND_MAX / range;

    if (random < RAND_MAX - remain) {
        return low + random / slot;
    } else {
        return random_int (low, high);
    }
}

/* wall-clock time in seconds for POSIX-compliant clocks */
double wctime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec + 1E-6 * tv.tv_usec);
}

/* printf if PRINTMODE is defined */
void dbg_printf(const char *fmt, ...)
{
    #ifdef PRINTMODE
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    #endif
}

void printArray(long *A, int lo, int hi){
    int i;

    printf("[");
    for (i = lo; i <= hi ; i++) {
        printf("%ld", A[i]);
        if ( i != hi) {
            printf(", ");
        }
    }
    printf("]\n");
}

/* print an array if PRINTMODE is defined */
void dbg_printArray(long *A, int lo, int hi) {
    #ifdef PRINTMODE
    printArray(A, lo, hi);
    #endif
}

/************************************************
 *  Sequential quicksort.
 ***********************************************/ 

int sequential_partition(long *array,int left,int right){
    long temp;
    long pivot = array[right];
    int i = left-1;
    int j;
    for(j=left; j<right;j++){
        if(array[j]<pivot){
            i++;
            temp = array[j];
            array[j]=array[i];
            array[i]= temp;
        }
    }
    array[right] = array[i+1];     
    array[i+1]=pivot;
    return i+1;
}

void sequential_quicksort(long *array,int left,int right){
    if(left<right){
        int splitPoint = sequential_partition(array,left, right);
        sequential_quicksort(array,left,splitPoint-1);
        sequential_quicksort(array,splitPoint+1,right);
    }
}
