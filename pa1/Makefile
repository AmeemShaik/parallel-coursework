all:
	
	gcc nbodies.c -lm -lrt -o nbodies
	gcc nbodies.c -lm -lrt -O3 -o nbodies_compiler_opts

	gcc -DNEWTONSTHIRD nbodies.c -lm -lrt -o nbodies_newton
	gcc -DNEWTONSTHIRD nbodies.c -lm -lrt -O3 -o nbodies_newton_compiler_opts
