//
//  MemoryBenchmark.c
//  Memory Benchmark
//
//  Created by Kunal Dhande on 2/2/16.
//  Copyright © 2016 Kunal Dhande. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <string.h>
#include <limits.h>

#define randomChar "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"[random () % 36]

int noOfThreads;
long blockSize;
double throughPut, latency;
long constBlockSize;

long randomNumber();
double calculateTimeDiff(struct timeval start, struct timeval end);
void *readWriteRandom();
void *readWriteSequential();

int main (int argc,  char** argv){
    int i;
    
    if(argc < 3) {
        printf("Run with valid arguements:\n1. Type: (seq/rand)\n2. No of Threads (1/2) 3. Block size (1/1024/1048576)");
        return 0;
    }
    
    noOfThreads = (int) strtol(argv[2], NULL, 0);
    pthread_t pth[4];	// this is our thread identifier
    blockSize = strtol(argv[3], NULL, 0);
    constBlockSize = 100 * 1024 * 1024;

    
    if(strcmp(argv[1], "Sequential") == 0)
        for(i=0; i<noOfThreads; i++)
            pthread_create(&pth[i],NULL,readWriteSequential, "");
    else if(strcmp(argv[1], "Random") == 0)
        for(i=0; i<noOfThreads; i++)
            pthread_create(&pth[i],NULL,readWriteRandom, "");
    else {
        printf("Invalid Mode..");
        return 0;
    }
    
    for(i=0; i<noOfThreads; i++)
        pthread_join(pth[i], NULL);

    
    printf("\n  %s\t%3d\t%7ld\t\t%10ld\t%10ld\t%12f MB/sec\t%02.15f ms",argv[1], noOfThreads, blockSize, constBlockSize, constBlockSize/blockSize, throughPut, latency/noOfThreads);
    return 1;
}



// Read write Operation : Random
void *readWriteRandom() {
    int i, j;
    char *readBlock, *writeBlock;
    char *readBlock_t, *writeBlock_t;
    
    long innerIterations = constBlockSize/blockSize;
    long totalBData = innerIterations*blockSize;
    struct timeval diff;
    struct timeval start;
    double result1, result2;
    
    // Populating bloack data with chars
    readBlock = malloc(constBlockSize * sizeof(char));
    writeBlock = malloc(constBlockSize * sizeof(char));
    
    for(i=0; i< constBlockSize; i++) {
        *(readBlock + i) = randomChar;
        *(writeBlock + i) = randomChar;
    }
    
    readBlock_t = readBlock;
    writeBlock_t = writeBlock;
    
    // Calculating throughput
    gettimeofday(&start,NULL);
    
    for (j=0; j<innerIterations; j++) {
        if(j%2==0) {
            // Setting readBlock pointer to random position from begining
            readBlock = readBlock_t + randomNumber();
            writeBlock = writeBlock_t + randomNumber();
        }
        else {
            // Setting readBlock pointer to random position from end
            readBlock = readBlock_t + (constBlockSize - randomNumber() - blockSize);
            writeBlock = writeBlock_t + (constBlockSize - randomNumber() - blockSize);
        }
        
        memcpy(writeBlock, readBlock, blockSize);   // Reading blockSize data from readBlock and copying to WriteBlock
    }
    
    gettimeofday(&diff,NULL);
    result1 = (totalBData/(1024.0*1024.0))/calculateTimeDiff(start, diff);
    throughPut += result1;
    
    // Calculating latency
    gettimeofday(&start,NULL);
    for (j=0; j<innerIterations; j++) {
        writeBlock = writeBlock_t + randomNumber(); // Setting writeBlock pointer to random position
        memset(writeBlock, 'a', blockSize);         // Setting blocksize data to writeblock
    }
    
    gettimeofday(&diff,NULL);
    
    result2 = calculateTimeDiff(start, diff)*1000/totalBData;
    latency += result2;
    return NULL;
}

// Read write Operation : Sequential
void *readWriteSequential() {
    int i, j;
    char *readBlock, *writeBlock;
    char *readBlock_t, *writeBlock_t;
    long innerIterations = constBlockSize/blockSize;
    struct timeval diff;
    struct timeval start;
    long totalBData = innerIterations*blockSize;
    double result1, result2;
    
    // Populating bloack data with chars
    readBlock = malloc(constBlockSize * sizeof(char));
    writeBlock = malloc(constBlockSize * sizeof(char));
    
    for(i=0; i< constBlockSize; i++) {
        *(readBlock + i) = randomChar;
        *(writeBlock + i) = randomChar;
    }
    readBlock_t = readBlock;
    writeBlock_t = writeBlock;

    // Calculating throughput
    gettimeofday(&start,NULL);
    
    for (j=0; j<innerIterations; j++) {
        memcpy(writeBlock, readBlock, blockSize); // Reading blockSize data from readBlock and copying
        readBlock += blockSize;                         // Maintaining readBlock pointer the position for sequential access
        writeBlock += blockSize;                        // Maintaining writeBlock pointer the position for sequential access

    }
    
    readBlock = readBlock_t;
    writeBlock = writeBlock_t;

    
    gettimeofday(&diff,NULL);
    double sec = calculateTimeDiff(start, diff);
    result1 = totalBData/(1024.0*1024.0*sec);
    throughPut += result1;
    
    
    // Calculating latency
    gettimeofday(&start,NULL);
    for (j=0; j<innerIterations; j++) {
        memset(writeBlock, 'a', blockSize);             // Setting blocksize data to writeblock
        writeBlock += blockSize;                        // Maintaining writeBlock pointer the position for sequential access
    }
    
    gettimeofday(&diff,NULL);
    result2 = calculateTimeDiff(start, diff)*1000/totalBData;
    latency += result2;
    return NULL;
}


// Function to get randowm number to access memory randomly
long randomNumber()  {
    return labs(rand()% (constBlockSize - blockSize));
}

// Calculating time difference
double calculateTimeDiff(struct timeval start, struct timeval end) {
    double msec = (double)((end.tv_sec - start.tv_sec)*1000000L
                           +end.tv_usec) - start.tv_usec;
    
    
    return msec/1000000.0;
}
