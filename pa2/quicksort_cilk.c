#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

// Array size at which to degrade to insertion sort.
#define SERIAL_INSERTION_NSIZE 16
#define SERIAL_QUICKSORT_NSIZE 512
long *lt,*gt;

void dbg_printf(const char *fmt, ...)
{
    #ifdef PRINTMODE
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    #endif
}

// Random int from [low, high)
long random_int (unsigned int low, unsigned int high)
{
  int random = rand();
  if (RAND_MAX == random) return random_int(low, high);
  int range = high - low,
      remain = RAND_MAX % range,
      slot = RAND_MAX / range;
  if (random < RAND_MAX - remain) {
    return low + random / slot;
  } else {
    return random_int (low, high);
  }
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
/* Inclusive, in-place parallel prefix sum. 
    Takes an input X[1..n], n=2^k and produces
    an output S[1..n], a vector of prefix sums.
*/
void parallel_prefix_sum(long *S, int left, int n, int k) {

    int i, h;
    for(h = 1 ; h <= k ; h++) {
        cilk_for (i = 1; i <= (n >> h); i++) {
            S[left+i * (1 << h) - 1] += S[left+(1 << h) * i - (1 << (h-1)) - 1];
        }
    }

    for (h = k ; h >= 1; h--) {
        cilk_for(i = 2; i <= (n >> (h-1)); i++){
            if (i % 2) {
                S[left+i * (1 << (h-1)) -1] = S[left+i * (1 << (h-1)) - (1 << (h-1)) - 1] + S[left+i * (1 << (h-1)) -1];
            }
        }
    }

}
void serial_prefix_sum(long *S, int left, int n){
    int i;
    int sum = 0;
    for(i = 0; i < n; i++){
      sum+=S[left+i];
      S[left+i] = sum;
    }
}
void serial_insertionSort(long *array, int left, int right) {

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

int serial_partition(long *array,int left,int right){
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

void serial_quicksort(long *array,int left,int right){
    if(left<right){
        int splitPoint = serial_partition(array,left, right);
        serial_quicksort(array,left,splitPoint-1);
        serial_quicksort(array,splitPoint+1,right);
    }
}

void quicksort_recursive(long *array,int left,int right, long* copyArray){

    dbg_printf("quicksort(array, %d, %d)\n", left, right);

    if(left >= right) {
        return;
    }

    // First see if we've degraded to serial insertion sort.
    if (right - left + 1 < SERIAL_INSERTION_NSIZE) {
        return serial_insertionSort(array, left, right);
    }

    // Then if we've degraded to serial quicksort
    else if (right - left + 1 < SERIAL_QUICKSORT_NSIZE) {
        return serial_quicksort(array, left, right);
    }

    else {
        int splitPoint = partition(array,left, right,copyArray);
        dbg_printArray(array, left, right);
        cilk_spawn quicksort_recursive(array,left,splitPoint-1,copyArray);
        quicksort_recursive(array,splitPoint+1,right,copyArray);
       // cilk_sync;
    }
}

void quicksort(long *array, int size) {
    long *copyArray = (long *) malloc (sizeof(long) * size);
    lt = (long *) malloc (sizeof(long) * size);
    gt = (long *) malloc (sizeof(long) * size);
    quicksort_recursive(array, 0, size-1, copyArray);
   // printArray(array,0,size-1);
}

int partition(long *array, int left, int right, long *copyArray){
    // Compute n, k for helper prefix sum
    int n = (right - left + 1),
        k = (int) log2(n),
        i;

    // Get a random pivot
    i = random_int(left, right);
    long pivot = array[i];
    array[i] = array[right];
    array[right] = pivot;
    // Set flags in comparison flag arrays
    // Don't need eq anymore, just keep it in lt (except the pivot)
    cilk_for (i = left; i <= right; i++) {
        copyArray[i] = array[i];
        if (array[i] < pivot) {
            lt[i] = 1;
            gt[i] = 0;
        } else if(array[i]>pivot) {
            lt[i] = 0;
            gt[i] = 1;
        }
        else{
            if(i==right){
                lt[i]=0;
            }
            else {
                lt[i]=1;
            }
            gt[i]=0;
        }
    }
    parallel_prefix_sum(lt, left, n,k);
    parallel_prefix_sum(gt, left, n,k);
    int pivotIndex = left+lt[right];
    //add the pivot
    array[pivotIndex] = pivot;
    // Now use these mappings to swap in parallel
    // Note, we don't look at i = right, since its the pivot
    cilk_for (i = left; i < right; i++){
        if(copyArray[i]<=pivot){
            array[left+lt[i]-1] = copyArray[i];
        }
        else if (copyArray[i]>pivot){
            array[pivotIndex+gt[i]] = copyArray[i];
        }
    }
    return pivotIndex;
}

int main(int argc, char **argv) {

    clock_t start, stop;
    double time_elapsed;

    if(argc!=2){
        dbg_printf("Incorrect number of arguments, expected 1 argument\n");
        return EXIT_FAILURE;
    }

    int size = atoi(argv[1]);
    long *array;
    array = malloc(size*sizeof(long));
    int i;
    srand(time(NULL));

    for(i = 0; i < size; i++){
        long r = size-i;
        array[i] = r;
    }

    dbg_printf("Unsorted Array\n");
    dbg_printArray(array, 0, size-1);
    start = clock();
    quicksort(array, size);
    stop = clock();
    time_elapsed = (double) (stop - start) / CLOCKS_PER_SEC;

    dbg_printf("Sorted Array\n");
    dbg_printArray(array, 0, size-1);
    printf("Time elapsed for %d elements: %f seconds.\n", size, time_elapsed);
    return 0;
}
