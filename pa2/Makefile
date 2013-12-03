all:

	gcc -O3 quicksort.c -o quicksort -lm -std=c99

	gcc -DPRINTMODE quicksort.c -o quicksortPrint -std=c99

	gcc -O3 quicksort_cilk.c -o quicksort_cilk -lcilkrts -ldl -lm -fcilkplus -std=c99

	gcc -DPRINTMODE quicksort_cilk.c -o quicksort_cilk_print -lcilkrts -ldl -fcilkplus -std=c99
