import csv
import fileinput
import sys

"""
CSV headers
Algorithm,InputSize,Workers,SortTime
"""

field_names = ['Algorithm','InputSize','Workers','SortTime']

def main(argv):

    if len(argv) != 2:
        sys.stderr.write("Usage: %s [FILENAME]" % argv[0])
        exit(1)

    results = dict()
    counts = dict()

    filename = argv[1]
    with open(filename) as csvfile:
        datareader = csv.DictReader(csvfile, delimiter=',')
        for entry in datareader:

            lookupTuple = (entry['Algorithm'], entry['InputSize'], entry['Workers'])

            try:

                counts[lookupTuple] += 1
                results[lookupTuple] += float(entry['SortTime'])

            except KeyError:

                counts[lookupTuple] = 1
                results[lookupTuple] = float(entry['SortTime'])

    for lookupTuple in results:

        results[lookupTuple] = results[lookupTuple] / counts[lookupTuple]
    
    result_filename = ''.join([filename[:-4], "_averaged.csv"])
    with open(result_filename, "w") as output_file:
        writer = csv.DictWriter(output_file,fieldnames=field_names)
        writer.writeheader()
        for keyTuple, value in results.items():
            writer.writerow(
                {
                    'Algorithm': keyTuple[0],
                    'InputSize': keyTuple[1],
                    'Workers': keyTuple[2],
                    'SortTime': value
                }
            )


if __name__ == "__main__":
    main(sys.argv)
