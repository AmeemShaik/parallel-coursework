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
#define X_MAX 1024.0
#define Y_MIN 0.0
#define Y_MAX 1024.0
#define MASS_MIN 100
#define MASS_MAX 1000

int n, k;

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

    if (argc != 3) {
        printf("Expected 2 arguments: nBodies kSteps.\n");
        return EXIT_FAILURE;
    }

    n = atoi(argv[1]);
    k = atoi(argv[2]);

    if (n <= 0 || k <= 0) {
        printf("Expected non-zero values for n and k.\n");
        return EXIT_FAILURE;
    }

    b = (struct body *) malloc( n * sizeof(struct body));

    //Todo: parameterize timestep
    int t, timeStep = 1;

    // Initialize bodies
    int j;
    for(j=0; j < n; j++) {
        init(
            j,
            (double)rand() * (MASS_MAX - MASS_MIN) / (double)RAND_MAX + MASS_MIN,   
            (double)rand() * (X_MAX - X_MIN) / (double)RAND_MAX + X_MIN,
            (double)rand() * (Y_MAX - Y_MIN) / (double)RAND_MAX + Y_MIN,
            0.0,
            0.0
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
            b[i].r_x += timeStep * b[i].v_x;
            b[i].r_y += timeStep * b[i].v_y;
            b[i].v_x += timeStep * b[i].a_x;
            b[i].v_y += timeStep * b[i].a_y;
            printState(i);
        }
    }

    printf("Final states after %d steps:\n", k);

    for(j=0; j < n; j++) {
        printState(j);
    }

    return EXIT_SUCCESS;
};