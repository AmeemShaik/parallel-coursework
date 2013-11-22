#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

/* Inclusive, in-place parallel prefix sum. 
    Takes an input X[1..n], n=2^k and produces
    an output S[1..n], a vector of prefix sums.
*/
void parallel_prefix_sum(long *X, long *S, int n, int k) {

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

int main(int argc, char **argv) {


}


