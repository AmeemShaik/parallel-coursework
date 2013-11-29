#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

// Array size at which to degrade to insertion sort.
#define SERIAL_INSERTION_NSIZE 16
#define SERIAL_QUICKSORT_NSIZE 1024

short *lt_flags, *eq_flags, *gt_flags;
int *lt_indices, *eq_indices, *gt_indices;

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
int random_int (unsigned int low, unsigned int high)
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
void parallel_prefix_sum(short *X, int *S, int left, int n, int k) {

    int i, h;

    cilk_for(i = 1; i <= n; i++) {
         S[left + i - 1] = X[left + i - 1];
    }

    for(h = 1 ; h <= k ; h++) {
        cilk_for (i = 1; i <= ( n >> h) ; i++) {
            S[left + i * (1 << h) - 1] += S[left + (1 << h) * i - (1 << (h-1)) - 1];
        }
    }

    for (h = k ; h >= 1; h--) {
        cilk_for(i = 2; i <= (n >> (h-1)) ; i++){
            if (i % 2) {
                S[left + i * (1 << (h-1)) -1] = S[left + i * (1 << (h-1)) - (1 << (h-1)) - 1] + S[left + i * (1 << (h-1)) -1];
            }
        }
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

void serial_quicksort(long *array,int left,int right){
    if(left<right){
        int splitPoint = serial_partition(array,left, right);
        serial_quicksort(array,left,splitPoint-1);
        serial_quicksort(array,splitPoint+1,right);
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
        int splitPoint = partition(array,left, right, copyArray);
        dbg_printArray(array, left, right);
        cilk_spawn quicksort_recursive(array,left,splitPoint-1,copyArray);
        quicksort_recursive(array,splitPoint+1,right,copyArray);
    }
}

void quicksort(long *array, int size) {
    long *copyArray = (long *) malloc (sizeof(long) * size);
    quicksort_recursive(array, 0, size-1, copyArray);
}

int partition(long *array, int left, int right, long* copyArray){

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
    cilk_for (i = 0; i < n; i++) {
        if (array[left + i] < pivot) {
            lt_flags[i] = 1;
            eq_flags[i] = 0;
            gt_flags[i] = 0;
        } else if (array[left + i] == pivot) {
            lt_flags[i] = 0;
            eq_flags[i] = 1;
            gt_flags[i] = 0;
        } else {
            lt_flags[i] = 0;
            eq_flags[i] = 0;
            gt_flags[i] = 1;
        }
    }

    // Compute index mappings from the flag arrays and make them consecutive
    lt_flags[left] += left;
    parallel_prefix_sum(lt_flags, lt_indices, left, n, k);
    eq_flags[left] += lt_indices[n-1];
    parallel_prefix_sum(eq_flags, eq_indices, left, n, k);
    gt_flags[left] += eq_indices[n-1];
    parallel_prefix_sum(gt_flags, gt_indices, left, n, k);
    gt_flags[left] -= eq_indices[n-1];
    eq_flags[left] -= lt_indices[n-1];
    lt_flags[left] -= left;

    // Now use these mappings to swap in parallel
    cilk_for (i = left; i <= right; i++) {
        if ( lt_flags[i - left] ) {
            copyArray[lt_indices[i-left]] = array[i];
        } else if ( eq_flags [i - left] ) {
            copyArray[eq_indices[i-left]] = array[i];
        } else{
            copyArray[gt_indices[i-left]] = array[i];
        }
    }
    cilk_for (i = left; i <= right; i++) {
        array[i] = copyArray[i];
    }
    return eq_indices[n-1] - 1;

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

    lt_flags = (short *) malloc (sizeof(short) * size);
    eq_flags = (short *) malloc (sizeof(short) * size);
    gt_flags = (short *) malloc (sizeof(short) * size);

    lt_indices = (int *) malloc (sizeof(int) * size);
    eq_indices = (int *) malloc (sizeof(int) * size);
    gt_indices = (int *) malloc (sizeof(int) * size);

    int i;
    srand(time(NULL));

    for(i = 0; i < size; i++){
        long r = size - i;
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


