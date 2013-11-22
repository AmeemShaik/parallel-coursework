#include <stdio.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>


void printArray(long *A, int n){
    int i;

    printf("[");
    for (i = 0; i < n ; i++) {
        printf("%ld", A[i]);
        if ( i != n-1) {
            printf(", ");
        }
    }
    printf("]\n");
}

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

int main(int argc, char **argv) {

    // Test parallel prefix sum.
    // n = 8; k = 3
    long mynumbers[] = {1, 2, 3, 4, 5, 6, 7, 8};
    long mynumbers_pfx[8];

    printf("Before sum, mynumbers=\n");
    printArray(mynumbers, 8);
    parallel_prefix_sum(mynumbers, mynumbers_pfx, 8, 3);
    printf("After sum, mynumbers=\n");
    printArray(mynumbers, 8);
    printf("And the prefix sum array=\n");
    printArray(mynumbers_pfx, 8);

}


