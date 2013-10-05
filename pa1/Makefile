all:

	rm -f nbodies nbodies_newton 

	gcc -fopenmp -DTESTMODE nbodies.c -lm -O3 -o nbodies

	gcc -fopenmp -DTESTMODE -DNEWTONSTHIRD nbodies_rewrite.c -lm -O3 -o nbodies_rewrite

	gcc -fopenmp -DNEWTONSTHIRD -DTESTMODE nbodies.c -lm -O3 -o nbodies_newton

