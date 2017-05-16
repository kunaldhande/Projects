//
//  CPUBenchmark.c
//  CPU Benchmark
//
//  Created by Kunal Dhande on 2/1/16.
//  Copyright © 2016 Kunal Dhande. All rights reserved.
//


#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#define ITERATIONS 1000000000
#define TIMER_SEC 600


typedef enum { false, true } bool;

long samples[TIMER_SEC];
int counter = 1;
int noOfThreads;
long *noOfIterations;
double *timeInMicoSec;
bool flag;


double calculateTimeDiff(struct timeval start, struct timeval end);
void timer();
void *flops(void* index);
void *iops(void* index);

double calculateFlop(double msec, long noOfIterrations) {
    // GFLOPS or IOPS = (no of iterations/time in Seconds) * 10^9
    return ((double)noOfIterrations/(msec*1000.0));
}

int main (int argc,  char** argv){
    int i;
    bool isTimer = false;
    FILE *fp;
    double result = 0;
    long totalIterations = 0;
    double totalTime = 0;
    flag = true;
    
    
    if(argc < 3) {
        printf("Run with valid arguements:\n1. Type: (flops/iops)\n2. No of Threads (1/2/4)\n3. For 10 min run (0/1)(Optional - Default 0)");
        return 0;
    }
    
    if(argc == 4) {
        if((int) strtol(argv[3], NULL, 0) == 1)
            isTimer = true;
    }

    noOfThreads = (int) strtol(argv[2], NULL, 0);
    pthread_t pth[4];	// this is our thread identifier
    
    noOfIterations = malloc(noOfThreads * sizeof(long));
    timeInMicoSec = malloc(noOfThreads * sizeof(double));

    if(strcmp(*(argv + 1), "FLOPS") ==0) {
        for(i=0; i<noOfThreads; i++)
            pthread_create(&pth[i],NULL,flops, (void*) i);   // Thread for calculating flops
    }
    else if(strcmp(*(argv + 1), "IOPS")==0){
        for(i=0; i<noOfThreads; i++)
            pthread_create(&pth[i],NULL,iops,(void*) i);     // Thread for calculating iops
    }
    else {
        printf("Invalid type of operation");
        return 0;
    }
    
    
    if(isTimer)
        timer();
    
    for(i=0; i<noOfThreads; i++)
        pthread_join(pth[i], NULL);
    
    if(isTimer) {
        // Storing 10 min sample data into file
        fp = fopen(argv[1], "w");
        if(!fp) {
            printf("Error Occurred while creating file");
            return 0;
        }
        
        for (i=0; i<TIMER_SEC; i++) {
            double flops = calculateFlop(1000000.0, i==0 ? samples[i] : samples[i] - samples[i-1]);
            fprintf(fp, "%d\t%f\n", i+1, flops);
            printf("  %3d\t\t%f\n", i+1, flops);
        }
        fclose(fp);
    }
    else {
        for(i=0; i<noOfThreads; i++) {
            totalIterations += noOfIterations[i];
            totalTime += timeInMicoSec[i];
        }
        
        result = calculateFlop(totalTime, noOfThreads*totalIterations);
        printf("\n  %s\t%3d\t%ld\t%f\t  %f", argv[1], noOfThreads, totalIterations, totalTime/1000000.0, result);
    }
    
    free(noOfIterations);
    free(timeInMicoSec);
    return 1;
}

// Function To Calculate IOPS (Integer Operations Per Second)
void *iops(void* index) {
    // Declaration
    int threadIndex = (int)index;
    volatile long i,j;
    volatile int var1 = 1;
    volatile int var2 = 2;
    volatile int var3 = 3;
    volatile int var4 = 4;
    volatile int var5 = 5;
    volatile int var6 = 6;
    volatile int var7 = 7;
    volatile int var8 = 8;
    volatile int var9 = 9;
    volatile int var10 = 4;
    volatile int var11 = 0;
    volatile int var12 = 2;
    volatile int var13 = 7;
    volatile int var14 = 5;
    volatile int var15 = 7;
    volatile int var16 = 5;
    struct timeval diff;
    struct timeval start;
    
    noOfIterations[threadIndex] = 0;
    
    
    gettimeofday(&start,NULL);
    
    // Loop to calculate IOPS
    for(i=0;i<ITERATIONS && flag;i++) {
        for(j=0; j<counter && flag; j++){
            var1 = var1 * 3;
            var2 = var2 * 5;
            var3 = var3 * 0;
            var4 = var4 * 2;
            var5 = var5 * 7;
            var6 = var6 * 2;
            var7 = var7 * 6;
            var8 = var8 * 4;
            var9 = var9 + 6;
            var10 = var10 * 0;
            var11 = var11 * 2;
            var12 = var12 * 8;
            var13 = var13 * 1;
            var14 = var14 * 5;
            var15 = var15 * 6;
            var16 = var16 * 3;
            
            noOfIterations[threadIndex] += 40;  //  Total Arithmetic Oprations  : 16
                                                //  Total Assignments           : 16
                                                //  Increment noOfIterations    : 2
                                                //  Counter                     : 2
                                                //  Condition Checker           : 4
            
        }
        noOfIterations[threadIndex] += 9;       //  Condition Checker               : 2
                                                //  Increment noOfIterations        : 2
                                                //  Counter                         : 2
                                                //  Condition Checker (Inner Loop)  : 2
                                                //  Setting counter to zero (Inner loop) : 1
    }
    noOfIterations[threadIndex] += 3;           //  Condition Checker  : 2
                                                //  Setting counter to zero : 1
    
    gettimeofday(&diff,NULL);
    
    // Saving total time to execute in timeInMicoSec
    timeInMicoSec[threadIndex] = calculateTimeDiff(start, diff);
    return NULL;
}


// Function To Calculate FLOPS (Floating Operations Per Second)
void *flops(void* index) {
    // Declaration
    int threadIndex = (int)index;
    volatile double i,j;
    volatile double var1 = 6.2;
    volatile double var2 = 2.4;
    volatile double var3 = 4.7;
    volatile double var4 = 1.3;
    volatile double var5 = 9.5;
    volatile double var6 = 0.8;
    volatile double var7 = 9.5;
    volatile double var8 = 3.1;
    volatile double var9 = 2.9;
    volatile double var10 = 7.7;
    volatile double var11 = 1.4;
    volatile double var12 = 6.5;
    volatile double var13 = 8.3;
    volatile double var14 = 4.2;
    volatile double var15 = 3.3;
    volatile double var16 = 1.2;
    struct timeval diff;
    struct timeval start;
    
    noOfIterations[threadIndex] = 0;

    gettimeofday(&start,NULL); // Start time
    
    
    for(i=0;i<ITERATIONS && flag;i++) {
        for(j=0; j<counter && flag; j++){
            var1 = var1 * 3.2;
            var2 = var2 * 5.3;
            var3 = var3 * 0.4;
            var4 = var4 * 2.5;
            var5 = var5 * 7.5;
            var6 = var6 * 2.1;
            var7 = var7 * 6.3;
            var8 = var8 * 4.3;
            var9 = var9 * 6.4;
            var10 = var10 * 0.3;
            var11 = var11 * 2.3;
            var12 = var12 * 8.9;
            var13 = var13 * 1.0;
            var14 = var14 * 5.4;
            var15 = var15 * 6.2;
            var16 = var16 * 5.2;
            
            noOfIterations[threadIndex] += 40;  //  Total Arithmetic Oprations  : 16
                                                //  Total Assignments           : 16
                                                //  Increment noOfIterations    : 2
                                                //  Counter                     : 2
                                                //  Condition Checker           : 4
        }
        noOfIterations[threadIndex] += 9;
    }
    noOfIterations[threadIndex] += 3;
    
    gettimeofday(&diff,NULL); // End time
    
    // Saving total time to execute in timeInMicoSec
    timeInMicoSec[threadIndex] = calculateTimeDiff(start, diff);
    return NULL;
}


// A function that runs about 10 mins and for each second check for number of operation executed
void timer() {
    int count = 0;
    counter = 20;
    double time_counter = 0;
    int i;
    
    struct timeval this_time;
    gettimeofday(&this_time,NULL);
    struct timeval last_time = this_time;
    
    
    for (;;) {
        gettimeofday(&this_time,NULL);
        
        time_counter = calculateTimeDiff(last_time, this_time);
        
        // time_counter is equal to or greater than 1 sec, then get no of operations executed by each thread..
        if(time_counter >= 1000000.0)
        {
            last_time = this_time;
            
            for(i=0;i<noOfThreads; i++)
                samples[count] += noOfIterations[i]; // Storing sum of no of operations in samples
            
            count++;
            if(count ==TIMER_SEC) {  // Stop execution after 600 seconds
                counter=1;
                flag = false;
                break;
            }
        }
    }
}


// Function To calculate time difference
double calculateTimeDiff(struct timeval start, struct timeval end) {
    double msec = (double)((end.tv_sec - start.tv_sec)*1000000L
                           +end.tv_usec) - start.tv_usec;
    
    
    return msec;
}