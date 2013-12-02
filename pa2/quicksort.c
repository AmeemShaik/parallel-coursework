#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define LOWLIMIT 100000

void quicksort(long *array,int left,int right);
int partition(long *array,int left,int right);
void printArray(long *A, int lo, int hi);

static unsigned int size;

/* wall-clock time in seconds for POSIX-compliant clocks */
double wctime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec + 1E-6 * tv.tv_usec);
}

int main(int argc, char **argv)
{

    double start, stop;
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
    start = wctime();
    quicksort(array, left, right);
    stop = wctime();
    time_elapsed = (double) (stop - start);

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
        int splitPoint = partition(array,left, right);
        quicksort(array,left,splitPoint-1);
        quicksort(array,splitPoint+1,right);
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
