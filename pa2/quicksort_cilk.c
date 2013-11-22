#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

void quicksort(long long*,int,int);
int partition(long long*,int,int);
void printArray(long long*, int size);

/* Inclusive, in-place parallel prefix sum. 
    Takes an input X[1..n], n=2^k and produces
    an output S[1..n], a vector of prefix sums.
*/
void parallel_prefix_sum(long *X, long *S, int n, int k) {

    assert (n == (1 << k));
    assert (X != NULL && S != NULL);

    int i, h;

    cilk_for(i = 1; i <= n; i++) {
        S[i - 1] = X[i - 1];
    }

    printf("Just initialized S\n");
    printArray(S, n);

    for(h = 1 ; h <= k ; h++) {
        cilk_for (i = 1; i <= ( n >> h) ; i++) {
            printf("i*2^h = %d\n", i*(1 << h));
            printf("(2^(h-1) = %d\n", (1 << (h-1)));
            // S[i * (1 << h)] = S[i * (1 << h) - (1 << (h-1))] + S[i * (i << h)];
            S[i * (1 << h) - 1] += S[(1 << h) * i - (1 << (h-1)) - 1];
        }
    }

    printf("First loop done\n");
    printArray(S, n);

    for (h = k ; h >= 1; h--) {
        cilk_for(i = 2; i <= (n >> (h-1)) ; i++){
            if (i % 2) {
                S[i * (1 << (h-1)) -1] = S[i * (1 << (h-1)) - (1 << (h-1)) - 1] + S[i * (1 << (h-1)) -1];
            }
        }
    }

}

static unsigned int size;
int main(int argc, char **argv)
{
    if(argc!=2){
        printf("Incorrect number of arguments, expected 1 argument\n");
        return EXIT_FAILURE;
    }
    size = atoi(argv[1]);
    long long *array;
    array = malloc(size*sizeof(long long));
    int i;
    srand(time(NULL));
    for(i = 0; i < size; i++){
        long long r = rand()%size;
        array[i] = r;
    }
    #ifdef PRINTMODE
    printf("Unsorted Array\n");
    printArray(array,size);
    #endif

    int left = 0;
    int right = size-1;
    quicksort(array, left, right);
    #ifdef PRINTMODE
    printf("Sorted Array\n");
    printArray(array,size);
    #endif
    return 0;
}
void printArray(long long *array,int size){
    int i;
    printf("[");
    for(i=0;i<size;i++){
        printf("%lld,",array[i]);
    }
    printf("]\n");
}
void quicksort(long long *array,int left,int right){
    //select the first element as pivot
    if(left<right){
    	#ifdef PARALLEL
        int splitPoint = partition(array,left, right);
        #pragma omp task
        quicksort(array,left,splitPoint-1);
        #pragma omp task
        quicksort(array,splitPoint+1,right);
        #else
        int splitPoint = partition(array,left, right);
        quicksort(array,left,splitPoint-1);
        quicksort(array,splitPoint+1,right);
        #endif
    }
}
int partition(long long *array,int left,int right){
    int pivotIndex = (rand()%(right+1-left))+left;
    long long temp = array[pivotIndex];
    array[pivotIndex] = array[right];
    array[right] = temp;
    long long pivot = array[right];
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
