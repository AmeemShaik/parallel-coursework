all:
	
	gcc nbodies.c -lm -o nbodies
	gcc nbodies.c -lm -O3 -o nbodies_compiler_opts

	gcc -DNEWTONSTHIRD nbodies.c -lm -o nbodies_newton
	gcc -DNEWTONSTHIRD nbodies.c -lm -O3 -o nbodies_newton_compiler_opts
