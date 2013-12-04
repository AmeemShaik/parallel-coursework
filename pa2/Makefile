all:

	gcc -O3 quicksort.c -o quicksort -lm

	gcc -O3 -DPRINTMODE quicksort.c -o quicksortPrint -lm

	gcc -O3 quicksort_cilk.c -o quicksort_cilk -lcilkrts -ldl -lm -fcilkplus

	gcc -O3 -DPRINTMODE quicksort_cilk.c -o quicksort_cilk_print -lcilkrts -ldl -lm -fcilkplus
