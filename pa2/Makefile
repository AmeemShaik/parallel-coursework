all:

	gcc quicksort.c -o quicksort

	gcc -DPRINTMODE quicksort.c -o quicksortPrint

	gcc -fopenmp -DPARALLEL -DPRINTMODE quicksort.c -o qParallelPrint

	gcc -fopenmp -DPARALLEL quicksort.c -o qParallel 

	gcc prefix_sum.c -o prefix_sum -lcilkrts -ldl -fcilkplus
