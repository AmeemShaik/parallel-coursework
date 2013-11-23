#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

// Array size at which to degrade to insertion sort.
#define SIZE_DEGRADE_PARAM 0

void printArray(long *A, int n){
    #ifdef PRINTMODE
    int i;

    printf("[");
    for (i = 0; i < n ; i++) {
        printf("%ld", A[i]);
        if ( i != n-1) {
            printf(", ");
        }
    }
    printf("]\n");
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

void quicksort(long *array,int left,int right){

    if(left >= right) {
        return;
    }

    // Degrade to insertion sort.
    if (right - left + 1 < SIZE_DEGRADE_PARAM) {
        insertionSort(array, left, right);
        return;
    }

    int splitPoint = partition(array,left, right);
    #ifdef PRINTMODE
    printf("partition done, returned splitpoint =%d\n", splitPoint);
    #endif
    cilk_spawn quicksort(array,left,splitPoint-1);
    quicksort(array,splitPoint+1,right);
    cilk_sync;
}
int partition(long *array, int left, int right){
    
    #ifdef PRINTMODE
    printf("============================================\n");
    printf("partition(array, %d, %d)\n", left, right);
    #endif

    int n = (right - left + 1);
    int k = (int) log2(n);

    // printf("n=%d, k=%d\n", n, k);

    long result[n];

    // Flag arrays for less than, greater than, equal to.
    short int lt[n],
              eq[n],
              gt[n];

    int lt_indices[n],
        eq_indices[n],
        gt_indices[n];

    int i;
    cilk_for (i = 0; i < n; i++) {
        result[i] = 0;
    }

    // Swap a random member into the rightmost slot
    i = left + rand() % (right - left + 1);
    long copy = array[i];
    array[i] = array[right];
    array[right] = copy;

    long pivot = array[right];
    // printf("pivot=%d\n", pivot);
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

    #ifdef PRINTMODE
    printf("lt_index_max = %d\n", lt_index_max);
    printf("eq_index_max = %d\n", eq_index_max);
    #endif

    cilk_for (i = 0; i < n; i++) {
        if (lt[i]) {
            result[lt_indices[i] - 1] = array[i];
        } else if (eq[i]) {
            result[eq_indices[i] + lt_index_max - 1] = array[i];
        } else if (gt[i]) {
            result[gt_indices[i] + eq_index_max - 1] = array[i];
        }
    }

    cilk_for (i = 0; i < n; i++) {
        array[i] = result[i];
    }

    printArray(result, n);

    return eq_index_max - 1;

}

int main(int argc, char **argv) {

    if(argc!=2){
            printf("Incorrect number of arguments, expected 1 argument\n");
            return EXIT_FAILURE;
        }
        int size = atoi(argv[1]);
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


