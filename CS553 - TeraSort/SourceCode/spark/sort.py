from __future__ import print_function
import sys
from pyspark import SparkContext


def toPlainText(data):
    line = list('  '.join(str(d) for d in data))
    line.extend(['\n'])
    return "".join(line)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: sort <input file> <output file>", file=sys.stderr)
        exit(-1)
    sc = SparkContext(appName="Data Sorting..")
    lines = sc.textFile("hdfs://localhost:9000/" + sys.argv[1], 1)

    sortedCount = lines.flatMap(lambda x: x.split('\r\n')) \
        .map(lambda x: (x[:10], x[12:])) \
        .sortByKey() \
        .map(toPlainText)

    sortedCount.saveAsTextFile("hdfs://localhost:9000/" + sys.argv[2])

    sc.stop()
