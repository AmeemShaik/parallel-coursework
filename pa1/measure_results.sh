#!/bin/bash
DEFAULT_TIMESTEP=0.001
DEFAULT_STEPS=1000

TIMESTEP=${1:-$DEFAULT_TIMESTEP}
STEPS=${2:-$DEFAULT_STEPS}
P_VALUES=(1 2 4 8 10 12 14 15 16)
N_VALUES=(100 200 500 1000 2000 5000 10000)

echo "Compiling programs"
make

rm -f results.csv
touch results.csv

echo "Running simulations with varying (n, p, k):" >> results.csv
echo "  Time step size t=$TIMESTEP" >> results.csv

echo "n, p, k, newtons, walltime" >> results.csv
for p in ${P_VALUES[*]}
do
    for n in ${N_VALUES[*]}
    do
        export OMP_NUM_THREADS=$p
        ./nbodies $n $TIMESTEP $STEPS >> results.csv
        ./nbodies_newton $n $TIMESTEP $STEPS >> results.csv
        #each will print (n, p, k, newtons{0,1}, walltime)
    done
done
