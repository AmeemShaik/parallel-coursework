all:

	gcc -O3 quicksort.c -o quicksort -lm

	gcc -DPRINTMODE quicksort.c -o quicksortPrint

	gcc -O3 quicksort_cilk.c -o quicksort_cilk -lcilkrts -ldl -lm -fcilkplus

	gcc -DPRINTMODE quicksort_cilk.c -o quicksort_cilk_print -lcilkrts -ldl -fcilkplus
