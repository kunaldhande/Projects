#!/bin/sh

cpu=0
mem=0
disk=0

if [ "$1" = "cpu" ]; then
    mem=1
    disk=1
elif [ "$1" = "memory" ]; then
    cpu=1
    disk=1
elif [ "$1" = "disk" ]; then
    cpu=1
    mem=1
fi



if [ $cpu = 0 ]; then
    gcc CPUBenchmark.c -o CPUBenchmark -pthread

    echo "CPU Benchmarks:"
    echo "---------------------------------------------------------------"
    echo -e "  Type\tThreads\tIterations\tTime(Sec)\tGFLOPS/GIOPS"
    echo "---------------------------------------------------------------"
    ./CPUBenchmark FLOPS 1
    ./CPUBenchmark FLOPS 2
    ./CPUBenchmark FLOPS 4

    ./CPUBenchmark IOPS 1
    ./CPUBenchmark IOPS 2
    ./CPUBenchmark IOPS 4

    echo ""
    echo "---------------------------------------------------"

    echo "FLOPS: Sample Data For 10 Min"
    echo "-------------------------"
    echo -e "  Time(sec)\tGFLOPS"
    echo "-------------------------"
    ./CPUBenchmark FLOPS 4 1

    echo ""
    echo "IOPS: Sample Data For 10 Min"
    echo "-------------------------"
    echo -e "  Time(sec)\tGFLOPS"
    echo "-------------------------"
    ./CPUBenchmark IOPS 4 1


    echo ""

elif [ $mem = 0 ]; then
    echo ""


    gcc MemoryBenchmark.c -o MemoryBenchmark -lpthread

    echo "Memory BenchMark:"
    echo "------------------------------------------------------------------------------------------------------------------------"
    echo -e "   Type\t\tThreads\tBlock Size\t File size\tIterations\t\tThroughput\tLatency"
    echo "------------------------------------------------------------------------------------------------------------------------"

    ./MemoryBenchmark Sequential 1 1
    ./MemoryBenchmark Sequential 1 1024
    ./MemoryBenchmark Sequential 1 1048576
    ./MemoryBenchmark Sequential 2 1
    ./MemoryBenchmark Sequential 2 1024
    ./MemoryBenchmark Sequential 2 1048576


    ./MemoryBenchmark Random 1 1
    ./MemoryBenchmark Random 1 1024
    ./MemoryBenchmark Random 1 1048576
    ./MemoryBenchmark Random 2 1
    ./MemoryBenchmark Random 2 1024
    ./MemoryBenchmark Random 2 1048576
    echo ""
    echo "------------------------------------------------------------------------------------------------------------------------"
    echo "*All sizes are in Bytes..."

    echo ""

elif [ $disk = 0 ]; then
    echo ""


    gcc DiskBenchmark.c -o DiskBenchmark -lpthread

    echo "Disk BenchMark:"
    echo ""
    echo "Generating file for read opearations"
    base64 /dev/urandom | head -c 1610612736 > file
    echo ""

    echo "------------------------------------------------------------------------------------------------------------------------"
    echo -e "   Type\t\tOp\tThreads\tBlock Size\t File size\tIterations\tThroughput\tLatency"
    echo "------------------------------------------------------------------------------------------------------------------------"

# "Experiment 3A:7 - Sequential Disk Read"
    ./DiskBenchmark Sequential r 1 1
# "Experiment 3A:8 - Sequential Disk Read"
    ./DiskBenchmark Sequential r 1 1024
# "Experiment 3A:9 - Sequential Disk Read"
    ./DiskBenchmark Sequential r 1 1048576
# "Experiment 3A:10 - Sequential Disk Read"
    ./DiskBenchmark Sequential r 2 1
# "Experiment 3A:11 - Sequential Disk Read"
    ./DiskBenchmark Sequential r 2 1024
# "Experiment 3A:12 - Sequential Disk Read"
    ./DiskBenchmark Sequential r 2 1048576

# "Experiment 3B:7 - Random Disk Read"
    ./DiskBenchmark Random r 1 1
# "Experiment 3B:8 - Random Disk Read"
    ./DiskBenchmark Random r 1 1024
# "Experiment 3B:9 - Random Disk Read"
    ./DiskBenchmark Random r 1 1048576
# "Experiment 3B:10 - Random Disk Read"
    ./DiskBenchmark Random r 2 1
# "Experiment 3B:11 - Random Disk Read"
    ./DiskBenchmark Random r 2 1024
# "Experiment 3B:12 - Random Disk Read"
./DiskBenchmark Random r 2 1048576

# "Experiment 3A:1 - Sequential Disk Write"
    ./DiskBenchmark Sequential w 1 1
# "Experiment 3A:2 - Sequential Disk Write"
    ./DiskBenchmark Sequential w 1 1024
# "Experiment 3A:3 - Sequential Disk Write"
    ./DiskBenchmark Sequential w 1 1048576
# "Experiment 3A:4 - Sequential Disk Write"
    ./DiskBenchmark Sequential w 2 1
# "Experiment 3A:5 - Sequential Disk Write"
    ./DiskBenchmark Sequential w 2 1024
# "Experiment 3A:6 - Sequential Disk Write"
    ./DiskBenchmark Sequential w 2 1048576

# "Experiment 3B:1 - Random Disk Write"
    ./DiskBenchmark Random w 1 1
# "Experiment 3B:2 - Random Disk Write"
    ./DiskBenchmark Random w 1 1024
# "Experiment 3B:3 - Random Disk Write"
    ./DiskBenchmark Random w 1 1048576
# "Experiment 3B:4 - Random Disk Write"
    ./DiskBenchmark Random w 2 1
# "Experiment 3B:5 - Random Disk Write"
    ./DiskBenchmark Random w 2 1024
# "Experiment 3B:6 - Random Disk Write"
    ./DiskBenchmark Random w 2 1048576

    echo ""
    echo "------------------------------------------------------------------------------------------------------------------------"
    echo "*All sizes are in Bytes..."



fi
