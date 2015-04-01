"""
Creates data set of the given size. Our intent is to
produce dataset that looks like some kind of step function.

For --cpp files, first line of the dataset, tells the size.
pav_tester.c uses size of dataset as the first element.

"""
from optparse import OptionParser

import random, sys

parser = OptionParser()
parser.add_option("--random", action="store_true", dest="random", default=False)
parser.add_option("--cpp", action="store_true", dest="cpp", default=False)
opts, args = parser.parse_args()

datasize = int(sys.argv[1])
outfile_name = "dataset_" + str(datasize)

if opts.cpp:
    datapoints = [str(datasize)]
    outfile_name = "cpp_" + outfile_name
else:
    datapoints = []

for i in range(datasize):
    if opts.random:
        x = random.randint(1, datasize)
    else:
        x = random.randint(50 * (i/50), 50 * (i/50 + 1))
    datapoints.append(str(x))

if opts.random:
    outfile_name = "random_" + outfile_name

f = open(outfile_name, "w")
f.write("\n".join(datapoints))
f.close()

