all:

	gcc quicksort.c -o quicksort

	gcc -DPRINTMODE quicksort.c -o quicksortPrint

	gcc -fopenmp -DPARALLEL -DPRINTMODE quicksort.c -o qParallelPrint

	gcc -fopenmp -DPARALLEL quicksort.c -o qParallel 

	g++ prefix_sum.cpp -o prefix_sum -lcilkrts -ldl -fcilkplus
