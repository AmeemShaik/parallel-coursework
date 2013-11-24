#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define LOWLIMIT 100000

void quicksort(long *array,int left,int right);
int partition(long *array,int left,int right);
void printArray(long *A, int lo, int hi);

static unsigned int size;
int main(int argc, char **argv)
{

    clock_t start, stop;
    double time_elapsed;

    if(argc!=2){
        printf("Incorrect number of arguments, expected 1 argument\n");
        return EXIT_FAILURE;
    }
    size = atoi(argv[1]);
    long *array;
    array = malloc(size*sizeof(long));
    int i;
    srand(time(NULL));
    for(i = 0; i < size; i++){
        long r = rand()%size;
        array[i] = r;
    }
    #ifdef PRINTMODE
    printf("Unsorted Array\n");
    printArray(array,0, size-1);
    #endif

    int left = 0;
    int right = size-1;
    start = clock();
    #ifdef PARALLEL
    {
        #pragma omp parallel
        {
            #pragma omp single
            quicksort(array, left, right); 
        }
    }
    #else
    {
        quicksort(array, left, right);
    }
    #endif
    stop = clock();
    time_elapsed = (double) (stop - start) / CLOCKS_PER_SEC;

    #ifdef PRINTMODE
    printf("Sorted Array\n");
    printArray(array,0, size-1);
    #endif
    printf("Time elapsed for %d elements: %f seconds.\n", size, time_elapsed);
    return 0;
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

void dbg_printArray(long *A, int lo, int hi) {
    #ifdef PRINTMODE
    printArray(A, lo, hi);
    #endif
}

void quicksort(long *array,int left,int right){
    if(left<right){
        #ifdef PARALLEL
        int splitPoint = partition(array,left, right);
        if(splitPoint-left>LOWLIMIT)
        {
            #pragma omp task
            quicksort(array,left,splitPoint-1);
        }
        else
        {
            quicksort(array,left,splitPoint-1);
        }
        if(right-splitPoint>LOWLIMIT)
        {
            #pragma omp task
            quicksort(array,splitPoint+1,right); 
        }
        else
        { 
            quicksort(array,splitPoint+1,right);
        }
        #else
        int splitPoint = partition(array,left, right);
        quicksort(array,left,splitPoint-1);
        quicksort(array,splitPoint+1,right);
        #endif
    }
}
int partition(long *array,int left,int right){
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
