wget http://apache.spinellicreations.com/spark/spark-1.6.1/spark-1.6.1-bin-hadoop2.6.tgz
tar -xzvf spark-1.6.1-bin-hadoop2.6.tgz
mv spark-1.6.1-bin-hadoop2.6.tgz spark


echo "Now run ./hadoop-setup.sh s"


#export AWS_SECRET_ACCESS_KEY=PkXWUg8zpTUZwb1Gjexmwqq0CNOYkxzMmo0E8DzN export AWS_ACCESS_KEY_ID=AKIAJQF5PWIBP2PP4DYA

#./spark-ec2 -k aws_spark -i ~/Downloads/aws_spark.pem -s 1 --instance-type=t2.micro --region=us-west-2 launch "aa"



#./bin/hadoop jar ./share/hadoop/mapreduce/hadoop-*-examples*.jar teragen 100000 /user/hduser/gutenberg
#./bin/hadoop jar ./share/hadoop/mapreduce/hadoop-*-examples*.jar terasort /user/hduser/gutenberg /user/hduser/gutenberg-output

#http://apache.mirrors.lucidnetworks.net/spark/spark-1.6.1/spark-1.6.1-bin-hadoop2.6.tgz
#http://downloads.lightbend.com/scala/2.11.8/scala-2.11.8.tgz