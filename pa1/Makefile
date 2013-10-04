all:

	rm -f nbodies nbodies_parallel nbodies_newton nbodies_newton_parallel

	gcc -DTESTMODE nbodies.c -lm -O3 -o nbodies	
	gcc -fopenmp -DTESTMODE nbodies.c -lm -O3 -o nbodies_parallel

	gcc -DNEWTONSTHIRD -DTESTMODE nbodies.c -lm -O3 -o nbodies_newton
	gcc -fopenmp -DNEWTONSTHIRD -DTESTMODE nbodies.c -lm -O3 -o nbodies_newton_parallel
