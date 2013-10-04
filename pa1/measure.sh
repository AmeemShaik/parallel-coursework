#!/bin/bash

DEFAULT_NUM_BODIES=5000
DEFAULT_TIMESTEP=0.001
DEFAULT_STEPS=4

NUM_BODIES=${1:-$DEFAULT_NUM_BODIES}
TIMESTEP=${2:-$DEFAULT_TIMESTEP}
STEPS=${3:-$DEFAULT_STEPS}
P_VALUES=(1 2 4 8 16)

BOLD_SEP="=============================================================================="
THIN_SEP="------------------------------------------------------------------------------"

echo "Compiling program..."
make

echo $SEP
echo "Running simulations with parameters:"
echo "  Number of bodies N=$NUM_BODIES"
echo "  Time step size t=$TIMESTEP"
echo "  Number of steps k=$STEPS"
echo $SEP

for p in ${P_VALUES[*]}
do
    export OMP_NUM_THREADS=$p
    echo "Using p=$OMP_NUM_THREADS threads:"

    echo "Parallel without newtons."
    time ./nbodies_parallel $NUM_BODIES $TIMESTEP $STEPS
    echo "Parallel with newtons"
    time ./nbodies_newton_parallel $NUM_BODIES $TIMESTEP $STEPS

    echo $THIN_SEP
done