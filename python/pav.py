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

# Sample Command
# ~/spark-1.3.0/bin/spark-submit pav.py --file ../TestData/thousand.txt

import itertools

from optparse import OptionParser
#from pyspark import SparkContext

def poolOld(input, l, r):
    # TODO: Change to use the weighted average.
    new_point = sum(input[l: r+1], 0.0) / (r-l+1)
    for i in range(l, r+1):
        input[i] = new_point

def pool(values, weights, l, r,):
    #print values, weights, l, r
    new_point = sum(map(lambda x: values[x] * weights[x], range(l, r+1))) / sum(weights[l: r+1])
    values[l] = new_point
    weights[l] = sum(weights[l : r+1])
    
    return values[:l+1], weights[:l+1]

def poolAdjacentViolators(input):
    """
    Main function to solve the pool adjacent violator algorithm
    on the given array of data.

    """
    input = map(lambda x: float(x), input)
    weights = []
    output = []

    index = 0
    #print input
    while index < len(input):
        #print "Enter: ", index, output, weights
        temp = index
        
        # Find monotonicity violating sequence, if any.   
        # Difference of temp-beg would be our violating range.
        while temp < len(input) - 1 and input[temp] > input[temp + 1]:
            # Append this number to the final output and set its weight to be 1.
            temp += 1
        
        if temp == index:
            output_beg = len(output) - 1
            output_end = output_beg + 1
            output.append(input[index])
            weights.append(1)
            #print index, output, weights, output_beg, output_end
            index += 1
        else:
            # Pool the violating sequence, if after violating monotonicity 
            # is broken, we need to fix the output array.
            output_beg = len(output)
            output_end = output_beg + temp - index
            output.extend(input[index: temp+1])
            weights.extend([1] * (temp-index+1))
            #print index, output, weights, output_beg, output_end
            index = temp + 1

        while output_beg >= 0 and output[output_beg] > output[output_beg + 1]:
            output, weights = pool(output, weights, output_beg, output_end)
            diff = (output_end - output_beg)
            output_beg -= 1
            output_end -= diff
            

        #print "Exit: ", index, output, weights

    return list(itertools.chain(*map(lambda i: [output[i]] * weights[i] , range(len(weights)))))

def poolAdjacentViolatorsOld(input):
    """
    Main function to solve the run pool adjacent violator algorithm 
    on the given array of data.

    >>> input = [1, 11, 8, 7, 6, 7, 13, 12, 11, 8, 9, 10, 4, 8]
    >>> result = poolAdjacentViolators(input)
    >>> print result
    [1, 7.8, 7.8, 7.8, 7.8, 7.8, 9.375000000000002, 9.375000000000002, 9.375000000000002, 9.375000000000002, 9.375000000000002, 9.375000000000002, 9.375000000000002, 9.375000000000002]
    
    """ 
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
                poolOld(input, i, j)
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
    parser.add_option("--partitions", dest="partitions", default=1)

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
        f.close()
        lines = map(lambda x: float(x.strip()), lines[1:])
        result = poolAdjacentViolatorsOld(lines)
        print result
        #f = open("chakkde", "w")
        #f.write(str(result))
        #f.close()
    else:
        sc = SparkContext("local", "PAV")
        textData = sc.textFile(opts.filename, use_unicode=False, minPartitions=int(opts.partitions))
        parallelPoolAdjacentViolators(textData)
