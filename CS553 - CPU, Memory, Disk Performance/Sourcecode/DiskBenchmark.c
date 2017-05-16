//
//  DiskBenchmark.c
//  Disk Benchmark
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
#define ITERATIONS 100
#define FILENAME "file"

char operation;
int noOfThreads;
long blockSize;
char *bufferBlock;
double throughPut, latency;
long constBlockSize;


int randomNumber();
double calculateTimeDiff(struct timeval start, struct timeval end);
void writeRandom();
void writeSequential();
void readRandom();
void readSequential();
void *sequentialOperations();
void *randomOperations();


int main (int argc,  char** argv){
    int i;
    char *buffer = NULL;
    FILE *fp;
    
    if(argc < 3) {
        printf("Run with valid arguements:\n1. Type: (seq/rand)\n2. Read/Write (r/d)\n3. No of Threads (1/2) 4. Block size (1/1024/1048576)");
        return 0;
    }
    
    noOfThreads = (int) strtol(argv[3], NULL, 0);
    pthread_t pth[4];	// this is our thread identifier
    blockSize = strtol(argv[4], NULL, 0);
    operation = argv[2][0];
    constBlockSize = 500 * 1024 * 1024 ;
    
    bufferBlock = malloc(blockSize * sizeof(char));
    
    
    fp = fopen(FILENAME, "r");
    if(!fp) {
        printf("Plase execute this command: base64 /dev/urandom | head -c 1610612736 > file");
        return 0;
    }
    fclose(fp);
    
    for(i=0; i< blockSize; i++) {
        *(bufferBlock + i) = randomChar;
    }
    
    if(strcmp(argv[1], "Sequential") == 0) {
        for(i=0; i<noOfThreads; i++)
            pthread_create(&pth[i],NULL,sequentialOperations, "");
    }
    else if(strcmp(argv[1], "Random") == 0) {
        for(i=0; i<noOfThreads; i++)
            pthread_create(&pth[i],NULL,randomOperations, "");
    }
    else {
        printf("\nInvalid Mode");
        return 0;
    }
    
    
    for(i=0; i<noOfThreads; i++)
        pthread_join(pth[i], NULL);
    
    
    printf("\n  %s\t%s\t%3d\t%7ld\t\t%10ld\t%d\t%12f MB/sec\t%02.15f ms",argv[1], operation=='r' ? "Read" : "Write", noOfThreads, blockSize, constBlockSize, ITERATIONS, throughPut/noOfThreads, latency/noOfThreads);
    free(bufferBlock);
    free(buffer);
    return 1;
}



// Write Operation : Random
void writeRandom() {
    int i;
    
    long totalBData = ITERATIONS*blockSize;
    double result1, result2;
    struct timeval diff;
    struct timeval start;
    
    gettimeofday(&start,NULL);
    FILE *fp = fopen(FILENAME, "w");
    if(!fp) {
        printf("Unable to open file for read operation");
        return;
    }
    size_t a;
    for(i=0; i<ITERATIONS; i++) {
        fflush(fp);
        fseek(fp, randomNumber(), SEEK_SET); // Seek file pointer to random position from starting of file
        a = fwrite(bufferBlock, sizeof(char), blockSize, fp);
        if(a!=blockSize)
            printf("\nError while writing");
    }
    fclose(fp);
    gettimeofday(&diff,NULL);
    
    result1 = (totalBData/(1024.0*1024.0))/calculateTimeDiff(start, diff);
    throughPut += result1;
    
    result2 = calculateTimeDiff(start, diff)*1000/totalBData;
    latency += result2;
}


// Write Operation : Sequential
void writeSequential() {
    int i;
    
    long totalBData = ITERATIONS*blockSize;
    double result1, result2;
    struct timeval diff;
    struct timeval start;
    
    gettimeofday(&start,NULL);
    FILE *fp = fopen(FILENAME, "w");
    if(!fp) {
        printf("Unable to open file for read operation");
        return;
    }
    
    size_t a;
    for(i=0; i<ITERATIONS; i++) {
        fflush(fp);
        a = fwrite(bufferBlock, sizeof(char), blockSize, fp);
        if(a!=blockSize)
            printf("\nError while writing");
    }
    
    fclose(fp);
    gettimeofday(&diff,NULL);
    
    result1 = (totalBData/(1024.0*1024.0))/calculateTimeDiff(start, diff);
    throughPut += result1;
    
    result2 = calculateTimeDiff(start, diff)*1000/totalBData;
    latency += result2;
}



// Read Operation : Random
void readRandom() {
    int i;
    long totalBData = ITERATIONS*blockSize;
    double result1, result2;
    struct timeval diff;
    struct timeval start;
    
    gettimeofday(&start,NULL);
    FILE *fp = fopen(FILENAME, "r");
    if(!fp) {
        printf("Unable to open file for read operation");
        return;
    }
    
    size_t a;
    for(i=0; i<ITERATIONS; i++) {
        fseek(fp, randomNumber(), SEEK_SET); // Seek file pointer to random position from starting of file
        a = fread(bufferBlock, sizeof(char), blockSize, fp);
        if(a!=blockSize)
            printf("\nError while reading");
    }
    
    fclose(fp);
    gettimeofday(&diff,NULL);
    
    result1 = (totalBData/(1024.0*1024.0))/calculateTimeDiff(start, diff);
    throughPut += result1;
    
    result2 = calculateTimeDiff(start, diff)*1000/totalBData;
    latency += result2;
}


// Read Operation : Sequential
void readSequential() {
    int i;
    
    long totalBData = ITERATIONS*blockSize;
    double result1, result2;
    struct timeval diff;
    struct timeval start;
    
    gettimeofday(&start,NULL);
    FILE *fp = fopen(FILENAME, "r");
    if(!fp) {
        printf("Unable to open file for read operation");
        return;
    }
    
    size_t a;
    for(i=0; i<ITERATIONS; i++) {
        a = fread(bufferBlock, sizeof(char), blockSize, fp);
        if(a!=blockSize)
            printf("\nError while reading");
    }
    
    fclose(fp);
    gettimeofday(&diff,NULL);
    
    result1 = (totalBData/(1024.0*1024.0))/calculateTimeDiff(start, diff);
    throughPut += result1;
    
    result2 = calculateTimeDiff(start, diff)*1000/totalBData;
    latency += result2;
}


void *sequentialOperations() {
    switch (operation) {
        case 'r':
            readSequential();
            break;
            
        case 'w':
            writeSequential();
            break;
    }
    return NULL;
}

void *randomOperations() {
    switch (operation) {
        case 'r':
            readRandom();
            break;
            
        case 'w':
            writeRandom();
            break;
    }
    return NULL;
}

// Function to get randowm number to access disk randomly
int randomNumber()  {
    return rand()% (constBlockSize- blockSize);
}

// Calculating time difference
double calculateTimeDiff(struct timeval start, struct timeval end) {
    double msec = (double)((end.tv_sec - start.tv_sec)*1000000L
                           +end.tv_usec) - start.tv_usec;
    
    
    return msec/1000000.0;
}

