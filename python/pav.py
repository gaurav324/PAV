# This is a spark implementation of the linear time implementaion of the PAV algorithm.
#
# Pool Violators Algorithm (PAV) is used to solve the problem of 
# isotonic linear regression.
#
# References:
# 1. "Active set algorithms for isotonic regression; A unifying framework", Michael J. Best, Nilotpal Chakravarti
# Url: http://download.springer.com/static/pdf/206/art%253A10.1007%252FBF01580873.pdf?auth66=1424891878_5426240d2e489679e9b1125537ab4295&ext=.pdf 
#
# 2. An approach to Parallelizing Isotonic Regression.
# Url: http://softlib.rice.edu/pub/CRPC-TRs/reports/CRPC-TR96640.pdf
#
# 3. Spark Implementaion in Scala
# URl: http://www.cakesolutions.net/teamblogs/isotonic-regression-implementation-in-apache-spark
# I have tried to take a lot of code from here.
#

from optparse import OptionParser
from pyspark import SparkContext

def pool(input, l, r):
    # TODO: Change to use the weighted average.
    new_point = sum(input[l: r+1], 0.0) / (r-l+1)
    for i in range(l, r+1):
        input[i] = new_point

count = 0
def poolAdjacentViolators(input):
    """
    Main function to solve the run pool adjacent violator algorithm 
    on the given array of data.

    >>> input = [1, 11, 8, 7, 6, 7, 13, 12, 11, 8, 9, 10, 4, 8]
    >>> result = poolAdjacentViolators(input)
    >>> print result
    [1, 7.8, 7.8, 7.8, 7.8, 7.8, 9.375000000000002, 9.375000000000002, 9.375000000000002, 9.375000000000002, 9.375000000000002, 9.375000000000002, 9.375000000000002, 9.375000000000002]
    
    """ 
    global count
    count += 1
    print "Count: ", count, " and size is ", len(input)  ,"\n"
    input = map(lambda x: float(x), input)
    i = 0
    while(i < len(input)):
        j = i

        # Find monotonicity violating sequence, if any.
        while j < len(input) - 1 and input[j] > input[j+1]:
            j += 1

        # If monotonicity is not violated, move to next point.
        if i == j:
            i += 1
        else:
            # Pool the violating sequence, if after violating monotonicity 
            # is broken, we need to go back again.
            # TODO: I am not sure that how is this linear in time. 
            while i >= 0 and input[i] > input[i + 1]:
                pool(input, i, j)
                i -= 1

            i = j

    return input

def parallelPoolAdjacentViolators(input):
    parallelResult = input.glom().flatMap(poolAdjacentViolators).collect()
    poolAdjacentViolators(parallelResult)    
    #print parallelResult

def get_opts():
    parser = OptionParser()
    parser.add_option("--serial", action="store_true", dest="serial", default=False)
    parser.add_option("--file", dest="filename")

    (options, args) = parser.parse_args()
    return options, args

if __name__ == "__main__":
    #import doctest
    #doctest.testmod()
    #textData = sc.parallelize([1, 11, 8, 7, 6, 7, 13, 12, 11, 8, 9, 10, 4, 8])
    opts, args = get_opts()
    if opts.serial:
        f = open(opts.filename, "r")
        lines = f.readlines()
        lines = map(lambda x: float(x.strip()), lines)
        print poolAdjacentViolators(lines)
    else:
        sc = SparkContext("local", "PAV")
        textData = sc.textFile(opts.filename, use_unicode=False)
        parallelPoolAdjacentViolators(textData)
