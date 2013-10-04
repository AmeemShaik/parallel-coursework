all:

	rm -f nbodies nbodies_newton 

	gcc -fopenmp -DTESTMODE -DPRINTMODE nbodies.c -lm -O3 -o nbodies

	gcc -fopenmp -DNEWTONSTHIRD -DTESTMODE -DPRINTMODE nbodies.c -lm -O3 -o nbodies_newton
