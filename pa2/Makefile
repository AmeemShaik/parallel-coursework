all:

	gcc -O3 quicksort.c -o quicksort -lm -std=c99

	gcc -DPRINTMODE quicksort.c -o quicksortPrint

	gcc -fopenmp -DPARALLEL -DPRINTMODE quicksort.c -o qParallelPrint

	gcc -fopenmp -DPARALLEL quicksort.c -o qParallel 

	gcc -O3 quicksort_cilk.c -o quicksort_cilk -lcilkrts -ldl -lm -std=c99 -fcilkplus

	gcc -DPRINTMODE quicksort_cilk.c -o quicksort_cilk_print -lcilkrts -ldl -fcilkplus
