all:

	rm -f nbodies nbodies_newton 

	gcc -pg -fopenmp -DTESTMODE  nbodies.c -lm -O3 -o nbodies

	gcc -pg -fopenmp -DNEWTONSTHIRD -DTESTMODE  nbodies.c -lm -O3 -o nbodies_newton
