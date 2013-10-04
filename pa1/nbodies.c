/* 
 * COMP633 - Programming Assignment 1(a)
 * Zach Cross (zcross@cs.unc.edu)
 * Ameem Shaik (shaik@cs.unc.edu)
 *
 *  n-bodies simulation (sequential)
 *      Usage: nbodies [number of bodies] [timestep] [number of steps to simulate]
 * compilation options:
 *      See Makefile. Executable names are self-explanatory.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <omp.h>

#define G 1.0
#define X_MIN 0.0
#define X_MAX 1.0
#define Y_MIN 0.0
#define Y_MAX 1.0
#define MASS_MIN 0.125
#define MASS_MAX 0.975
#define INIT_V_MIN -1.0
#define INIT_V_MAX 1.0
#define ONE_BILLION 1000000000L
unsigned short n, k;
double timestep;

struct body
{
    double m;      // Mass
    double r_x;    // X component of position
    double r_y;    // Y component of position
    double v_x;    // X component of velocity
    double v_y;    // Y component of velocity
    double *f_x;
    double *f_y;
};

struct body *b;

#ifdef NEWTONSTHIRD
void compute_forces() {
    unsigned short i, j;

    b[i].f_x = (double *) malloc(omp_get_num_threads() * sizeof(double));
    b[i].f_y = (double *) malloc(omp_get_num_threads() * sizeof(double));

    // reset forces to 0 since we'll accumulate
    for(i = 0 ; i < n; i++) {
        b[i].f_x = {0};
        b[i].f_y = {0};
    }
    // compute fij for all i<j ... and update f on i and f on j
	
	//int ID = omp_get_thread_num();
	#pragma omp parallel for private(i,j) shared(b)

    unsigned short pi = omp_get_thread_num();

	for(i = 0 ; i < n; i++) {
		double fij_x, fij_y;
		double result_i_x = 0,
			   result_i_y = 0;
		double r_yi = b[i].r_y;
		double r_xi = b[i].r_x;
		double iMass = b[i].m;
		for(j=0; j < i; j++) {
			double r_yj = b[j].r_y;
			double r_xj = b[j].r_x;
			//trying to exploit the fact that multiplication is faster than division
			double invDistance = 1/((r_yj - r_yi)*(r_yj - r_yi) +
				(r_xj - r_xi)*(r_xj - r_xi));
			//this is a constant value that is same for fx and fy. Can reuse it
			double constantVal = (G * iMass * b[j].m)*invDistance*sqrt(invDistance);
			fij_x = constantVal*(r_xj - r_xi);
			fij_y = constantVal*(r_yj - r_yi);
			b[i].f_x[omp_get_thread_num()] += fij_x;
			b[i].f_y[omp_get_thread_num()] += fij_y;
			b[j].f_x[omp_get_thread_num()] -= fij_x;
			b[j].f_y[omp_get_thread_num()] -= fij_y;
		}
	}

}
#else
void compute_forces() {
    unsigned short i, j;
    // reset forces to 0 since we'll accumulate

    b[i].f_x = (double *) malloc(1 * sizeof(double));
    b[i].f_y = (double *) malloc(1 * sizeof(double));

    for(i = 0; i < n; i++) {
        *b[i].f_x = 0;
        *b[i].f_y = 0;
    }
    // compute fij for all i,j where i!=j
	#pragma omp parallel for private(i,j)
    for(i = 0; i < n; i++) {
        double result_x = 0;
        double result_y = 0;
		double r_yi = b[i].r_y;
		double r_xi = b[i].r_x;
		double iMass = b[i].m;
		for(j=0; j < i; j++) {
			double r_yj = b[j].r_y;
			double r_xj = b[j].r_x;
			double invDistance = 1/((r_yj - r_yi)*(r_yj - r_yi) +
				(r_xj - r_xi)*(r_xj - r_xi));
			double constantVal = (G * iMass * b[j].m)*invDistance*sqrt(invDistance);
			result_x += constantVal*(r_xj - r_xi); 
			result_y += constantVal*(r_yj - r_yi);
		}
        for(j=i+1; j < n; j++) {
            double r_yj = b[j].r_y;
            double r_xj = b[j].r_x;
            double invDistance = 1/((r_yj - r_yi)*(r_yj - r_yi) +
                (r_xj - r_xi)*(r_xj - r_xi));
            double constantVal = (G * iMass * b[j].m)*invDistance*sqrt(invDistance);
            result_x += constantVal*(r_xj - r_xi); 
            result_y += constantVal*(r_yj - r_yi);
        }

		*b[i].f_x = result_x;
		*b[i].f_y = result_y;
    }
}
#endif

void init(unsigned short i, double m, double ri0_x, double ri0_y, double vi0_x, double vi0_y) {
    b[i].m = m;
    b[i].r_x = ri0_x;
    b[i].r_y = ri0_y;
    b[i].v_x = vi0_x;
    b[i].v_y = vi0_y;
}

void printState(unsigned short i) {
    printf("[body %d] m(%f) r(%f, %f) v(%f, %f)\n",
        i,
        b[i].m,
        b[i].r_x,
        b[i].r_y,
        b[i].v_x,
        b[i].v_y
    );
}
int main(int argc, char **argv) {

    if (argc != 4) {
        printf("Expected 3 arguments: numBodies timeStep numSteps.\n");
        return EXIT_FAILURE;
    }

    n = atoi(argv[1]);
    timestep = strtod(argv[2], NULL);
    k = atoi(argv[3]);

    printf("Received parameters:\n\t# Bodies: %d\n\tTime Step: %f\n\t# Steps:%d\n",
           n, timestep, k);

    if (n <= 0 || k <= 0 || timestep <= 0) {
        printf("Expected non-zero values for n, ∆t, and k.\n");
        return EXIT_FAILURE;
    }

    b = (struct body *) malloc( n * sizeof(struct body));

    //Todo: parameterize timestep
    unsigned short t;

    // Initialize bodies
    unsigned short j;
	
	//for test mode. Can remove later
	double testM[] = {0.577852,0.919489};
	double testrx[] = {0.458650,0.679296};
	double testry[] = {0.755605,0.678865};
	double testvx[] = {-0.649317, -0.495775};
	double testvy[] = {-0.478834, -0.798279};
	#pragma omp parallel for private(j)
    for(j=0; j < n; j++) {

        double vx = (double)rand() * (INIT_V_MAX - INIT_V_MIN) / (double)RAND_MAX + INIT_V_MIN;
        double vy = (double)rand() * (INIT_V_MAX - INIT_V_MIN) / (double)RAND_MAX + INIT_V_MIN;
        
        // Ensure |v| < 1
        double vmagSq = vx*vx + vy*vy;
        if( vmagSq >= 1 ) {
            vx = vx/vmagSq - 0.00125;
            vy = vy/vmagSq - 0.00125;
        }

        //for testing. Can remove later
		#ifdef TESTMODE
		double f_j = ((double)j)/n;
		init(
            j,
            1-f_j,
            f_j,
            f_j,
            -f_j,
            f_j
        );
		#else
		init(
            j,
            (double)rand() * (MASS_MAX - MASS_MIN) / (double)RAND_MAX + MASS_MIN,   
            (double)rand() * (X_MAX - X_MIN) / (double)RAND_MAX + X_MIN,
            (double)rand() * (Y_MAX - Y_MIN) / (double)RAND_MAX + Y_MIN,
            vx,
            vy
        );
		#endif
    }

    #ifdef PRINTMODE
    printf("Initial states:\n");
    for(j=0; j<n; j++)
        printState(j);
    #endif

    printf("Simulating...\n");
    // Integrate k steps
    for(t=1; t <= k; t++) {
        unsigned short i;
        // Compute forces on all bodies
        compute_forces();

        int p = omp_get_num_threads();

        for(i = 0; i < n ; i++){

            double fx, fy;
            fx = fy = 0;

            int j;
            #pragma omp parallel for private(j) reduction(+:fx)
            for(j=0; j < p; j++) {
                fx = fx + b[i].f_x[j];
            }

            #pragma omp parallel for private(j) reduction(+:fy)
            for(j=0; j < p; j++) {
                fy = fy + b[i].f_y[j];
            }

            b[i].r_x += timestep * b[i].v_x;
            b[i].r_y += timestep * b[i].v_y;
            b[i].v_x += timestep * fx/b[i].m;
            b[i].v_y += timestep * fy/b[i].m;
            #ifdef VERBOSE
            //printState(i);
            #endif
        }
    }
    //printf("Final states after %d steps:\n", k);
	#ifdef PRINTMODE
    for(j=0; j < n; j++) {
        printState(j);
    }
	#endif

    return EXIT_SUCCESS;
};