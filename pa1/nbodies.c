/* 
 * COMP633 - Programming Assignment 1(a)
 * Zach Cross (zcross@cs.unc.edu)
 * n-bodies simulation (sequential)
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

#define G 1.0
#define X_MIN 0.0
#define X_MAX 1.0
#define Y_MIN 0.0
#define Y_MAX 1.0
#define MASS_MIN 0.125
#define MASS_MAX 0.975
#define INIT_V_MIN -1.0
#define INIT_V_MAX 1.0
#define NEWTONSTHIRD

unsigned short n, k;
double timestep;

struct body
{
    double m;      // Mass
    double r_x;    // X component of position
    double r_y;    // Y component of position
    double v_x;    // X component of velocity
    double v_y;    // Y component of velocity
    double f_x;
    double f_y;
};

struct body *b;

/*probably the most expensive function, since it gets
called so often. Want to minize calls as much as possible
*/
/*
double dist(unsigned short i, unsigned short j) {
	double r_yj = b[j].r_y;
	double r_yi = b[i].r_y;
	double r_xj = b[j].r_x;
	double r_xi = b[i].r_x;
    return sqrt((r_yj - r_yi)*(r_yj - r_yi) +
        (r_xj - r_xi)*(r_xj - r_xi));
};

double _fx(unsigned short i, unsigned short j, double constantVal) {
	//made the distance a param to reduce calls to dist
	//d = dist(i,j)*dist(i,j)*dist(i,j);
    return constantVal*(b[j].r_x - b[i].r_x);
};

double _fy(unsigned short i, unsigned short j, double constantVal) {
	//made the distance a param to reduce calls to dist
	//d = dist(i,j)*dist(i,j)*dist(i,j);
    return constantVal*(b[j].r_y - b[i].r_y);
};
*/
#ifdef NEWTONSTHIRD
void compute_forces() {
    unsigned short i, j;
    double fij_x, fij_y;

    // reset forces to 0 since we'll accumulate
    for(i = 0 ; i < n; i++) {
        b[i].f_x = 0;
        b[i].f_y = 0;
    }

    // compute fij for all i<j ... and update f on i and f on j
    for(i = 0 ; i < n; i++) {
        double result_i_x = 0,
               result_i_y = 0;
		double r_yi = b[i].r_y;
		double r_xi = b[i].r_x;
		double iMass = b[i].m;
        for(j=0; j < i; j++) {
			double r_yj = b[j].r_y;
			double r_xj = b[j].r_x;
			double invDistance = 1/((r_yj - r_yi)*(r_yj - r_yi) +
				(r_xj - r_xi)*(r_xj - r_xi));
			double constantVal = (G * iMass * b[j].m)*invDistance*sqrt(invDistance);
            fij_x = constantVal*(r_xj - r_xi);
            fij_y = constantVal*(r_yj - r_yi);
            result_i_x += fij_x;
            result_i_y += fij_y;
            b[j].f_x -= fij_x;
            b[j].f_y -= fij_y;
        }

        b[i].f_x = result_i_x;
        b[i].f_y = result_i_y;
    }
}
#else
void compute_forces() {
    unsigned short i, j;
    double fij_x, fij_y;
    // reset forces to 0 since we'll accumulate
    for(i = 0; i < n; i++) {
        b[i].f_x = 0;
        b[i].f_y = 0;
    }
    // compute fij for all i,j where i!=j
    for(i = 0 ; i < n; i++) {

        double result_x = 0,
               result_y = 0;
		double r_yi = b[i].r_y;
		double r_xi = b[i].r_x;
		double iMass = b[i].m;
        for(j=0; j < n; j++) {
			double r_yj = b[j].r_y;
			double r_xj = b[j].r_x;
            double invDistance = 1/((r_yj - r_yi)*(r_yj - r_yi) +
				(r_xj - r_xi)*(r_xj - r_xi));
			double constantVal = (G * iMass * b[j].m)*invDistance*sqrt(invDistance);
            if(i==j)
                continue;
            result_x += constantVal*(r_xj - r_xi);
            result_y += constantVal*(r_yj - r_yi);
        }
		
		
        b[i].f_x = result_x;
        b[i].f_y = result_y;
    }
}
#endif

double init(unsigned short i, double m, double ri0_x, double ri0_y, double vi0_x, double vi0_y) {
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
	double testM[] = {0.577852,0.919489};
	double testrx[] = {0.458650,0.679296};
	double testry[] = {0.755605,0.678865};
	double testvx[] = {-0.649317, -0.495775};
	double testvy[] = {-0.478834, -0.798279};
    for(j=0; j < n; j++) {

        double vx = (double)rand() * (INIT_V_MAX - INIT_V_MIN) / (double)RAND_MAX + INIT_V_MIN;
        double vy = (double)rand() * (INIT_V_MAX - INIT_V_MIN) / (double)RAND_MAX + INIT_V_MIN;
        
        // Ensure |v| < 1
        double vmagSq = vx*vx + vy*vy;
        if( vmagSq >= 1 ) {
            vx = vx/vmagSq - 0.00125;
            vy = vy/vmagSq - 0.00125;
        }

        
		#ifdef TESTMODE
		init(
            j,
            testM[j],
            testrx[j],
            testry[j],
            testvx[j],
            testvy[j]
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
		
		#ifdef PRINTMODE
        printState(j);
		#endif
    }

    printf("Simulating...\n");

    // Integrate k steps
    for(t=1; t <= k; t++) {
        unsigned short i;
        // Compute forces on all bodies
        compute_forces();
        for(i = 0; i < n ; i++){
            b[i].r_x += timestep * b[i].v_x;
            b[i].r_y += timestep * b[i].v_y;
            b[i].v_x += timestep * b[i].f_x/b[i].m;
            b[i].v_y += timestep * b[i].f_y/b[i].m;
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