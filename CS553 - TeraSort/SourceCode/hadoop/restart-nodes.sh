#!/bin/bash

source ~/.bashrc

hdfs namenode -format
start-dfs.sh
start-yarn.sh
jps