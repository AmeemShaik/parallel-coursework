import sys
MAX_TRIALS = 10

def generate_cmds(problem_size, max_workers, trials):

    for _ in range(trials):
        print "./quicksort %d" % problem_size

    for workers in range(1, max_workers+1):
        for _ in range(trials):
            print "./quicksort_cilk %d %d" % (problem_size, workers)

def main(argv):

    if len(argv) < 3:
        sys.stderr.write("Usage: %s <MAX_WORKERS> <TRIALS>\n" % (argv[0]))
        return 1

    MAX_WORKERS = int(argv[1])

    # Allow up to MAX_TRIALS to be run per experiment, provided by argv[2]
    TRIALS = min( MAX_TRIALS, int(argv[2])) 
    PROBLEM_SIZES = [
        16000, 32000, 64000, 128000, 256000, 512000, 1024000, 2048000,
        4096000, 8192000, 16384000, 32768000, 65536000, 131072000
    ]

    print "####   run quicksort with varied n, p  ####"
    print "#BSUB -n 16"
    print "#BSUB -R \"span[hosts=1]\""
    print 'echo "Algorithm, Input Size,  Workers, Sort Time (seconds)"'

    for problem_size in PROBLEM_SIZES:
        generate_cmds(problem_size, MAX_WORKERS, TRIALS)

    print "##### end of parallel quicksort ####"

if __name__ == "__main__":
    main(sys.argv)
