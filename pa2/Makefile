all:

	gcc quicksort.c -o quicksort

	gcc -DPRINTMODE quicksort.c -o quicksortPrint

	gcc -fopenmp -DPARALLEL -DPRINTMODE quicksort.c -o qParallelPrint

	gcc -fopenmp -DPARALLEL quicksort.c -o qParallel 

	gcc quicksort_cilk.c -o quicksort_cilk -lcilkrts -ldl -fcilkplus

	gcc -DPRINTMODE quicksort_cilk.c -o quicksort_cilk_print -lcilkrts -ldl -fcilkplus
