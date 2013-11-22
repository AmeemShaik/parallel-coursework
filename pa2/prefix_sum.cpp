#include <iostream>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

/* Inclusive, in-place parallel prefix sum. 
    Takes an input X[1..n], n=2^k and produces
    an output S[1..n], a vector of prefix sums.
*/
void parallel_prefix_sum(long *X, long *S, int n, int k) {

    assert (n == (1 << k));

    cilk_for(int i = 0; i < n; i++) {
        S[i] = X[i];
    }

    for(int h = 0 ; h < k ; h++) {
        cilk_for (int i = 0; i < ( n >> h) ; i++) {
            S[i * (1 << h)] = S[i * (1 << h) - (1 << (h-1))] + S[i * (i << h)];
        }
    }

    for (int h = k-1 ; h >= 0; h--) {
        cilk_for(int i = 2; i < (n >> (h-1)) ; i++){
            if (i % 2) {
                S[i * (1 << (h-1))] = S[i * (1 << (h-1)) - (1 << (h-1))] + S[i * (1 << (h-1))];
            }
        }
    }

}

void printArray(long *A, int n){
    using namespace std;

    cout << "[";
    for (int i = 0; i < n ; i++) {
        cout << A[i];
        if ( i != n-1) {
            cout << ", ";
        }
    }
    cout << "]\n";
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


