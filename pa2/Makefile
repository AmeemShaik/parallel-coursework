
CILKPP  = cilk++
LIBARG  = -O1 -g -lcilkutil

all:

	gcc quicksort.c -o quicksort
	gcc quicksort.c -o quicksort_print -DPRINTMODE
	$(CILKPP) quicksort.cilk $(LIBARG) -o quicksort_cilk
	$(CILKPP) quicksort.cilk $(LIBARG) -DPRINTMODE -o quicksort_cilk_print
