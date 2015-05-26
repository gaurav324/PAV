# This is implementation of the MR algorithm mentioned in <> paper.
import itertools

from pyspark.mllib.classification import LogisticRegressionWithSGD
from pyspark.mllib.regression import LabeledPoint, LinearRegressionWithSGD, GeneralizedRegressionWithSGD
from pyspark import SparkConf, SparkContext

conf = SparkConf().setAppName("MR")
sc = SparkContext(conf=conf)

###########################################################################

class PAV(object):
    @classmethod
    def train(cls, data):
        output = data.glom().flatMap(PAV.poolAdjacentViolators).collect()
        output = PAV.poolAdjacentViolators(output)
        return output
    
    @classmethod
    def poolAdjacentViolators(cls, o_input):
        """
        Main function to solve the pool adjacent violator algorithm
        on the given array of data.
    
        This is a O(n) implementation. Trick is that while regersssing
        if we see a violation, we average the numbers and instead of
        storing them as two numbers, we store the number once and store
        a corresponding weight. This way, for new numbers we don't have 
        to go back for each n, but only one place behind and update the
        corresponding weights.
        """
        def pool(values, weights, l, r):
            new_point = sum(map(lambda x: values[x] * weights[x], range(l, r+1))) / sum(weights[l: r+1])
            values[l] = new_point
            weights[l] = sum(weights[l : r+1])

            return values[:l+1], weights[:l+1]


        input = map(lambda x: float(x.label), o_input)
        weights = []
        output = []
    
        index = 0
        while index < len(input):
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
                index += 1
            else:
                # Pool the violating sequence, if after violating monotonicity 
                # is broken, we need to fix the output array.
                output_beg = len(output)
                output_end = output_beg + temp - index
                output.extend(input[index: temp+1])
                weights.extend([1] * (temp-index+1))
                index = temp + 1
    
            # Fix the output to be in the increasing order.
            while output_beg >= 0 and output[output_beg] > output[output_beg + 1]:
                output, weights = pool(output, weights, output_beg, output_end)
                diff = (output_end - output_beg)
                output_beg -= 1
                output_end -= diff
                
        result = list(itertools.chain(*map(lambda i: [output[i]] * weights[i] , range(len(weights)))))
        for i, x in enumerate(result):
            o_input[i].label = result[i]
        return o_input

###########################################################################
###########################################################################

class MR(object):
    """
    """
    def __init__(self, data):
        # Data should be in the following format.
        # y_1 x_1 x_2,....x_n
        # y_2 x_1 x_2.....x_n
        # ..
        # y_n x_1 x_2.....x_n
        self.data = data

    def train(self, iterations=0):
        """
        Override the iterations parameter.

        """
        for i in range(iterations):
            self.data = PAV.train(self.data)
            self.data = sc.parallelize(self.data)
            model = GeneralizedRegressionWithSGD.train(self.data)
            self.data = self.data.map(lambda p: LabeledPoint(model.predict(p.features), p.features))

        return self.data
         
# Load and parse the data
def parse_point(line):
    values = [float(x) for x in line.split(' ')]
    return LabeledPoint(values[0], values[1:])

data = sc.textFile("/Users/gnanda/ConferenceCourseDataMining/spark-1.3.1-bin-hadoop2.4/data/mllib/sample_svm_data.txt", minPartitions=1)
print type(data)
parsed_data = data.map(parse_point)
mr = MR(parsed_data)
result = mr.train(2)
print result.collect()
