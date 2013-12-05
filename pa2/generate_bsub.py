"""
Generate a bsub file for quicksort performance measurement.

Usage: python generate_bsub.py | bsub
"""

import sys
from subprocess import call

MAX_TRIALS = 10

def generate_cmds(compiler, problem_size, max_workers, trials):

    for _ in range(trials):
        print "./quicksort_%s %d" % (compiler, problem_size)

    for workers in range(1, max_workers+1):
        for _ in range(trials):
            print "./quicksort_cilk_%s %d %d" % (compiler, problem_size, workers)

def main(argv):

    if len(argv) < 3:
        sys.stderr.write("Usage: %s <{gcc, icc}> <MAX_WORKERS> <TRIALS>\n" % (argv[0]))
        return 1

    # get compiler to use with Makefile
    COMPILER = argv[1].strip()
    if COMPILER not in ['gcc', 'icc']:
        sys.stderr.write("argument 1 should be in {gcc, icc}. Got %s.\n" % COMPILER)
        return 1

    # get number of workers to use
    MAX_WORKERS = int(argv[2])

    # Allow up to MAX_TRIALS to be run per experiment, provided by argv[2]
    TRIALS = min( MAX_TRIALS, int(argv[3]))
    PROBLEM_SIZES = [
        16000, 32000, 64000, 128000, 256000, 512000, 1024000, 2048000,
        4096000, 8192000, 16384000, 32768000, 65536000, 131072000
    ]

    sys.stderr.write("Using %s-compiled version of program.\n" % COMPILER)
    sys.stderr.write("Compiling program with %s...\n" % COMPILER)
    if call(['make', COMPILER]) != 0:
        sys.stderr.write("Compilation error. Aborting.\n")
        return 1

    sys.stderr.write("Using up to %d workers and %d trials per config.\n" % (MAX_WORKERS, TRIALS))
    sys.stderr.write("Generating bsub job...\n")

    print "####   run quicksort with varied n, p  ####"
    print "#BSUB -n 16"
    print "#BSUB -R \"span[hosts=1]\""
    print 'echo "Algorithm, Input Size,  Workers, Sort Time (seconds)"'

    for problem_size in PROBLEM_SIZES:
        generate_cmds(COMPILER, problem_size, MAX_WORKERS, TRIALS)

    print "##### end of parallel quicksort ####"

if __name__ == "__main__":
    main(sys.argv)
