#!/bin/sh

hadoop fs -mkdir /user/
hadoop fs -mkdir /user/hadoop/
hadoop fs -mkdir /user/hadoop/input/

echo "Copying data to hdfs"
hadoop fs -put /mnt/raid/input /user/hadoop/input/input
rm -rf input

hadoop com.sun.tools.javac.Main ~/HadoopSort.java
jar cf HadoopSort.jar HadoopSort*.class
hadoop jar HadoopSort.jar HadoopSort /user/hadoop/input/input /user/hadoop/output

#hadoop fs -cat /user/hadoop/output/part-r-00000

echo "Copying data to disk"
hadoop fs -get /user/hadoop/output ~/.