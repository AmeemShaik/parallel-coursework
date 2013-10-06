#!/bin/bash
DEFAULT_TIMESTEP=0.001
DEFAULT_STEPS=20000

TIMESTEP=${1:-$DEFAULT_TIMESTEP}
STEPS=${2:-$DEFAULT_STEPS}
P_VALUES=(1 2 4 8 16)
N_VALUES=(100 200 500 1000 2000 5000 10000)

echo "Running simulations with varying (n, p, k):"
echo "  Time step size t=$TIMESTEP"

echo "n, p, k, newtons, walltime"
for p in ${P_VALUES[*]}
do
    for n in ${N_VALUES[*]}
    do
        export OMP_NUM_THREADS=$p
        ./nbodies $n $TIMESTEP $STEPS
        ./nbodies_newton $n $TIMESTEP $STEPS
        #each will print (n, p, k, newtons{0,1}, walltime)
    done
done
