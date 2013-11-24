#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

// Array size at which to degrade to insertion sort.
#define SIZE_DEGRADE_PARAM 0

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
void parallel_prefix_sum(short int *X, int *S, int n, int k) {

    int i, h;

    cilk_for(i = 1; i <= n; i++) {
        S[i - 1] = X[i - 1];
    }

    for(h = 1 ; h <= k ; h++) {
        cilk_for (i = 1; i <= ( n >> h) ; i++) {
            // S[i * (1 << h)] = S[i * (1 << h) - (1 << (h-1))] + S[i * (i << h)];
            S[i * (1 << h) - 1] += S[(1 << h) * i - (1 << (h-1)) - 1];
        }
    }

    for (h = k ; h >= 1; h--) {
        cilk_for(i = 2; i <= (n >> (h-1)) ; i++){
            if (i % 2) {
                S[i * (1 << (h-1)) -1] = S[i * (1 << (h-1)) - (1 << (h-1)) - 1] + S[i * (1 << (h-1)) -1];
            }
        }
    }

}

void insertionSort(long *array, int left, int right) {

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

void quicksort_recursive(long *array,int left,int right, long* copyArray){

    dbg_printf("quicksort(array, %d, %d)\n", left, right);

    if(left >= right) {
        return;
    }

    // Degrade to insertion sort.
    if (right - left + 1 < SIZE_DEGRADE_PARAM) {
        insertionSort(array, left, right);
        return;
    }

    int splitPoint = partition(array,left, right, copyArray);
    dbg_printf("partition done, returned splitpoint =%d\n", splitPoint);
    dbg_printArray(array, left, right);
    cilk_spawn quicksort_recursive(array,left,splitPoint-1,copyArray);
    quicksort_recursive(array,splitPoint+1,right,copyArray);
    cilk_sync;
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

    // Flag and index arrays
    short int lt[n], eq[n], gt[n];
    int lt_indices[n], eq_indices[n], gt_indices[n];

    // Get a random pivot
    i = random_int(left, right);
    long pivot = array[i];
    array[i] = array[right];
    array[right] = pivot;
    dbg_printf("pivot = %ld\n", pivot);

    // Set flags in comparison flag arrays
    cilk_for (i = 0; i < n; i++) {
        if (array[left + i] < pivot) {
            lt[i] = 1;
            eq[i] = 0;
            gt[i] = 0;
        } else if (array[left + i] == pivot) {
            lt[i] = 0;
            eq[i] = 1;
            gt[i] = 0;
        } else {
            lt[i] = 0;
            eq[i] = 0;
            gt[i] = 1;
        }
    }

    dbg_printf("lt flags:\n");
    for(i=0; i < n; i++) {
        dbg_printf("%ld ", (long)lt[i]);
    }
    dbg_printf("\n");
    // dbg_printArray((long *) lt, 0, n-1);
    dbg_printf("eq flags:\n");
    for(i=0; i < n; i++) {
        dbg_printf("%ld ", (long)eq[i]);
    }
    dbg_printf("\n");
    dbg_printf("gt flags:\n");
    for(i=0; i < n; i++) {
        dbg_printf("%ld ", (long)gt[i]);
    }
    dbg_printf("\n");

    // Compute index mappings from the flag arrays and make them consecutive
    lt[0] += left;
    parallel_prefix_sum(lt, lt_indices, n, k);
    eq[0] += lt_indices[n-1];
    parallel_prefix_sum(eq, eq_indices, n, k);
    gt[0] += eq_indices[n-1];
    parallel_prefix_sum(gt, gt_indices, n, k);
    gt[0] -= eq_indices[n-1];
    eq[0] -= lt_indices[n-1];
    lt[0] -= left;

    dbg_printf("lt_indices:\n");
    for(i=0; i < n; i++) {
        dbg_printf("%ld ", (long)lt_indices[i]);
    }
    dbg_printf("\n");
    dbg_printf("eq_indices:\n");
    for(i=0; i < n; i++) {
        dbg_printf("%ld ", (long)eq_indices[i]);
    }
    dbg_printf("\n");
    dbg_printf("gt_indices:\n");
    for(i=0; i < n; i++) {
        dbg_printf("%ld ", (long)gt_indices[i]);
    }
    dbg_printf("\n");

    // Now use these mappings to swap in parallel
    cilk_for (i = left; i <= right; i++) {
        if ( lt[i - left] ) {
            copyArray[lt_indices[i-left] - 1] = array[i];
        } else if ( eq [i - left] ) {
            copyArray[eq_indices[i-left] - 1] = array[i];
        } else{
            copyArray[gt_indices[i-left] - 1] = array[i];
        }
    }

    cilk_for (i = left; i <= right; i++) {
        array[i] = copyArray[i];
    }

    return eq_indices[n-1] - 1;

}

int partition_bad(long *array, int left, int right, long* copyArray){
    
    dbg_printf("============================================\n");
    dbg_printf("partition(array, %d, %d)\n", left, right);

    int n = (right - left + 1);
    int k = (int) log2(n);

    // Flag arrays for less than, greater than, equal to.
    short int lt[n],
              eq[n],
              gt[n];

    int lt_indices[n],
        eq_indices[n],
        gt_indices[n];

    int i;

    // Swap a random member into the rightmost slot
    i = random_int(left, right);
    long copy = array[i];
    array[i] = array[right];
    array[right] = copy;

    long pivot = array[right];
    cilk_for (i = 0; i < n; i++) {
        if (array[i] < pivot) {
            lt[i] = 1;
            eq[i] = 0;
            gt[i] = 0;
        } else if (array[i] == pivot) {
            lt[i] = 0;
            eq[i] = 1;
            gt[i] = 0;
        } else {
            lt[i] = 0;
            eq[i] = 0;
            gt[i] = 1;
        }
    }

    parallel_prefix_sum(lt, lt_indices, n, k);
    parallel_prefix_sum(eq, eq_indices, n, k);
    parallel_prefix_sum(gt, gt_indices, n, k);

    int lt_index_max = lt_indices[n-1],
        eq_index_max = eq_indices[n-1] + lt_index_max;

    dbg_printf("pivot = %d\n", pivot);
    dbg_printf("lt_index_max = %d\n", lt_index_max);
    dbg_printf("eq_index_max = %d\n", eq_index_max);

    cilk_for (i = left; i <= right; i++) {
        if (lt[i-left]) {
            dbg_printf("array[%d]-->array[%d] %d < pivot\n", i, left + lt_indices[i] - 1, array[i]);
            copyArray[left + lt_indices[i-left] - 1] = array[i];
        } else if (eq[i-left]) {
            dbg_printf("array[%d]-->array[%d] %d == pivot\n", i, left + eq_indices[i] + lt_index_max - 1, array[i]);
            copyArray[left + eq_indices[i-left] + lt_index_max - 1] = array[i];
        } else if (gt[i-left]) {
            dbg_printf("array[%d]-->array[%d] %d > pivot\n", i, left + gt_indices[i] + eq_index_max - 1, array[i]);
            copyArray[left + gt_indices[i-left] + eq_index_max - 1] = array[i];
        }
    }

    cilk_for (i = left; i <= right; i++) {
        array[i] = copyArray[i];
    }

    return eq_index_max - 1;

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
        long r = size - i;
        array[i] = r;
        // array[i] = rand() % size*2;
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


