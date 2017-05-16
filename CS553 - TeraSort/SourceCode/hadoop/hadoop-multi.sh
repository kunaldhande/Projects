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
sudo chown ubuntu:ubuntu -R ~/hadoop/

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
        <value>hdfs://HadoopMaster:9000</value>
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
        <name>yarn.resourcemanager.resource-tracker.address</name>
        <value>HadoopMaster:8025</value>
    </property>
    <property>
        <name>yarn.resourcemanager.scheduler.address</name>
        <value>HadoopMaster:8035</value>
    </property>
    <property>
        <name>yarn.resourcemanager.address</name>
        <value>HadoopMaster:8050</value>
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


echo "Updating Master:"
cat > ~/hadoop/etc/hadoop/masters << EOL
HadoopMaster
EOL


cat > ~/hadoop/etc/hadoop/slaves << EOL
HadoopMaster
HadoopSlave1
HadoopSlave2
HadoopSlave3
HadoopSlave4
HadoopSlave5
HadoopSlave6
HadoopSlave7
HadoopSlave8
HadoopSlave9
HadoopSlave10
HadoopSlave11
HadoopSlave12
HadoopSlave13
HadoopSlave14
HadoopSlave15
HadoopSlave16
EOL
