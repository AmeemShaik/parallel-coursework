/* 
 * COMP633 - Programming Assignment 1(a)
 * Zach Cross (zcross@cs.unc.edu)
 * n-bodies simulation (sequential)
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define G 1.0
#define X_MIN 0.0
#define X_MAX 1.0
#define Y_MIN 0.0
#define Y_MAX 1.0
#define MASS_MIN 0.125
#define MASS_MAX 0.975
#define INIT_V_MIN -1.0
#define INIT_V_MAX 1.0

int n, k;
double timestep;

struct body
{
    double m;      // Mass
    double r_x;    // X component of position
    double r_y;    // Y component of position
    double v_x;    // X component of velocity
    double v_y;    // Y component of velocity
    double a_x;
    double a_y;
};

struct body *b;

double dist(int i, int j) {
    return sqrt((b[j].r_y - b[i].r_y)*(b[j].r_y - b[i].r_y) +
        (b[j].r_x - b[i].r_x)*(b[j].r_x - b[i].r_x));
};

double _fx(int i, int j) {
    double d = dist(i, j) * dist(i, j) * dist(i, j);
    return (G * b[i].m * b[j].m * (b[j].r_x - b[i].r_x))/d;
};

double _fy(int i, int j) {
    double d = dist(i, j) * dist(i, j) * dist(i, j);
    return (G * b[i].m * b[j].m * (b[j].r_y - b[i].r_y))/d;
};

double fx(int i) {
    double result = 0;
    int j;
    for(j = 0; j <= n; j++) {
        if (j == i) {
            continue;
        }
        result += _fx(i, j);
    }
    return result;
};

double fy(int i) {
    double result = 0;
    int j;
    for(j = 0; j <= n; j++) {
        if (j == i) {
            continue;
        }
        result += _fy(i, j);
    }
    return result;
};

double ax(int i) {
    return fx(i)/b[i].m;
}

double ay(int i) {
    return fy(i)/b[i].m;
}

double init(int i, double m, double ri0_x, double ri0_y, double vi0_x, double vi0_y) {
    b[i].m = m;
    b[i].r_x = ri0_x;
    b[i].r_y = ri0_y;
    b[i].v_x = vi0_x;
    b[i].v_y = vi0_y;
}

void printState(int i) {
    printf("[body %d] m(%f) r(%f, %f) v(%f, %f) a(%f, %f)\n",
        i,
        b[i].m,
        b[i].r_x,
        b[i].r_y,
        b[i].v_x,
        b[i].v_y,
        b[i].a_x,
        b[i].a_y
    );
}

int main(int argc, char **argv) {

    if (argc != 4) {
        printf("Expected 2 arguments: numBodies timeStep numSteps.\n");
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
    int t;

    // Initialize bodies
    int j;
    for(j=0; j < n; j++) {

        double vx = (double)rand() * (INIT_V_MAX - INIT_V_MIN) / (double)RAND_MAX + INIT_V_MIN;
        double vy = (double)rand() * (INIT_V_MAX - INIT_V_MIN) / (double)RAND_MAX + INIT_V_MIN;
        
        // Ensure |v| < 1
        double vmagSq = vx*vx + vy*vy;
        if( vmagSq >= 1 ) {
            vx = vx/vmagSq - 0.00125;
            vy = vy/vmagSq - 0.00125;
        }

        init(
            j,
            (double)rand() * (MASS_MAX - MASS_MIN) / (double)RAND_MAX + MASS_MIN,   
            (double)rand() * (X_MAX - X_MIN) / (double)RAND_MAX + X_MIN,
            (double)rand() * (Y_MAX - Y_MIN) / (double)RAND_MAX + Y_MIN,
            vx,
            vy
        );
        printState(j);
    }

    printf("Simulating...\n");

    // Integrate k steps
    for(t=1; t <= k; t++) {
        int i;

        for(i = 0; i < n ; i++){
            b[i].a_x = ax(i);
            b[i].a_y = ay(i);
        }

        for(i = 0; i < n ; i++){
            b[i].r_x += timestep * b[i].v_x;
            b[i].r_y += timestep * b[i].v_y;
            b[i].v_x += timestep * b[i].a_x;
            b[i].v_y += timestep * b[i].a_y;
            #ifdef VERBOSE
            printState(i);
            #endif
        }
    }

    printf("Final states after %d steps:\n", k);

    for(j=0; j < n; j++) {
        printState(j);
    }

    return EXIT_SUCCESS;
};