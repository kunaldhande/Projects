echo "Downlaoding hadoop"
cd
wget http://mirrors.sonic.net/apache/hadoop/common/hadoop-2.7.2/hadoop-2.7.2.tar.gz
tar -xzvf hadoop-2.7.2.tar.gz
mv hadoop-2.7.2 ~/hadoop

## Copy template of mapred-site.xml.template file
cp ~/hadoop/etc/hadoop/mapred-site.xml.template  ~/hadoop/etc/hadoop/mapred-site.xml

## Create Hadoop temp directories for Namenode and Datanode
mkdir -p /mnt/raid/hdfs/namenode
mkdir -p /mnt/raid/hdfs/datanode

## Assign ownership of this folder to Hadoop user
chown ubuntu:ubuntu -R ~/hadoop

cat << EOT >> ~/hadoop/etc/hadoop/hadoop-env.sh
## Update JAVA_HOME variable,
JAVA_HOME=/usr/lib/jvm/java-8-oracle
export HADOOP_CLIENT_OPTS="-XX:+UseConcMarkSweepGC $HADOOP_CLIENT_OPTS"
export HADOOP_CLIENT_OPTS="-Xmx20000M $HADOOP_CLIENT_OPTS"
EOT




cat > ~/hadoop/etc/hadoop/core-site.xml << EOL
<?xml version="1.0" encoding="UTF-8"?>
<?xml-stylesheet type="text/xsl" href="configuration.xsl"?>
<configuration>
	<property>
    	<name>fs.default.name</name>
    	<value>hdfs://localhost:9000</value>
	</property>
    <property>
        <name>hadoop.tmp.dir</name>
        <value>/mnt/raid/tmp/</value>
    </property>
</configuration>

EOL


cat > ~/hadoop/etc/hadoop/hdfs-site.xml << EOL
<?xml version="1.0" encoding="UTF-8"?>
<?xml-stylesheet type="text/xsl" href="configuration.xsl"?>
<configuration>
	<property>
    	<name>dfs.replication</name>
    	<value>1</value>
	</property>
	<property>
    	<name>dfs.namenode.name.dir</name>
    	<value>/mnt/raid/hdfs/namenode</value>
	</property>
    <property>
    	<name>dfs.datanode.data.dir</name>
    	<value>/mnt/raid/hdfs/datanode</value>
	</property>
</configuration>
EOL

cat > ~/hadoop/etc/hadoop/yarn-site.xml << EOL
<?xml version="1.0" encoding="UTF-8"?>
<?xml-stylesheet type="text/xsl" href="configuration.xsl"?>
<configuration>
	<property>
		<name>yarn.nodemanager.aux-services</name>
		<value>mapreduce_shuffle</value>
	</property>
	<property>
		<name>yarn.nodemanager.aux-services.mapreduce.shuffle.class</name>
		<value>org.apache.hadoop.mapred.ShuffleHandler</value>
	</property>
    <property>
        <name>map.cpu.vcores</name>
        <value>4</value>
    </property>
    <property>
        <name>reduce.cpu.vcores</name>
        <value>4</value>
    </property>
    <property>
        <name>yarn.app.mapreduce.am.resource.cpu-vcores</name>
        <value>4</value>
    </property>
</configuration>
EOL



cat > ~/hadoop/etc/hadoop/mapred-site.xml << EOL
<?xml version="1.0" encoding="UTF-8"?>
<?xml-stylesheet type="text/xsl" href="configuration.xsl"?>
<configuration>
</configuration>
EOL