all:

	rm nbodies nbodies_parallel nbodies_newton nbodies_newton_parallel

	gcc -DTESTMODE -DPRINTMODE nbodies.c -lm -O3 -o nbodies	
	gcc -fopenmp -DTESTMODE -DPRINTMODE nbodies.c -lm -O3 -o nbodies_parallel

	gcc -DNEWTONSTHIRD -DTESTMODE -DPRINTMODE nbodies.c -lm -O3 -o nbodies_newton
	gcc -fopenmp -DNEWTONSTHIRD -DTESTMODE -DPRINTMODE nbodies.c -lm -O3 -o nbodies_newton_parallel
