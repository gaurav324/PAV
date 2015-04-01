"""
Creates data set of the given size. Our intent is to
produce dataset that looks like some kind of step function.

First line of the dataset, tells the size.
"""
from optparse import OptionParser

import random, sys

parser = OptionParser()
parser.add_option("--random", action="store_true", dest="random", default=False)
opts, args = parser.parse_args()

datasize = int(sys.argv[1])

datapoints = [str(datasize)]
for i in range(datasize):
    if opts.random:
        x = random.randint(1, datasize)
    else:
        x = random.randint(50 * (i/50), 50 * (i/50 + 1))
    datapoints.append(str(x))

if opts.random:
    f = open("random_dataset_" + str(datasize), "w")
else:
    f = open("dataset_" + str(datasize), "w")
f.write("\n".join(datapoints))
f.close()

