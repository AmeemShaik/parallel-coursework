all:

	rm -f nbodies nbodies_newton 

	gcc -g -fopenmp -DTESTMODE -DPRINTMODE nbodies.c -lm -O3 -o nbodies

	gcc -g -fopenmp -DNEWTONSTHIRD -DTESTMODE -DPRINTMODE nbodies.c -lm -O3 -o nbodies_newton
