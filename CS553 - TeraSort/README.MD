Run Instructions:
==================
• To generate data:

	./gensort -a Size of data to be generated/100 <Destination_File>

	Eg:
		To generate 1 GB of data
		./gensort -a 10000000 ~/input


• To validate data:
	./valsort <Source_File>

	Eg:
		./valsort output




Setup:
======
1. Run Shared Memory Program:
	User has to generate data with input file name input in the same directory where code is present.

	./start.sh <Number_of_threads>

	Eg:
		./start.sh 1



2. Hadoop

	- Set up Hadoop Environment
		-  Run ./hadoop.sh <mode>	// mode is optional values (s: single-node and m: multi-node)
		   Eg:
			To setup single-node machine
			./hadoop.sh s

		- Starting Hadoop
			./start.sh

		- Generate input file and then to save into HDFS and run program
			./run.sh


3. Spark:
 	- Setup cluster with spark EC2 scrip:
		./spark/ec2/spark-ec2 -k <Key_name> -i <key_path> -s 2 --instance-type=<instance_type> --region=<region> launch <name_of_cluster>
	
		After setting up cluster:

		run ./bin/spark-sunbmit <Source code> <input_path> <output_path>	
		Eg:
			./bin/spark-sunbmit ~/sort.py /user/hadoop/input/input /user/hadoop/output 









Files in Folders
=================
1. “Outputs” folder contains output of sorted data for 1 GB and 1TB for shared memory, Hadoop, and Spark.
2. “screenshots” folder contains all screenshots related to AWS IP Addresses, running code and valsort.
3. “sourceCode” folder contains 
	- Gensort: Binaries for generating and validating data
	- hadoop: source code (HadoopSort.java) and scripts to setup and run hadoop on single and multiple nodes
	- sharedMemory: source code for shared memory program (C)
	- Spark: source code (sort.py) and script to setup Spark.
