all:

	rm -f nbodies nbodies_newton 

	gcc -pg -fopenmp -DPRINTMODE -DTESTMODE nbodies.c -lm -O3 -o nbodies

	gcc -pg -fopenmp -DPRINTMODE -DNEWTONSTHIRD -DTESTMODE nbodies.c -lm -O3 -o nbodies_newton

