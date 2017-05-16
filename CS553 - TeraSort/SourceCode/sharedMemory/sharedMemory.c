//
//  sharedMemory.c
//  sharedMemory
//
//  Created by Kunal Dhande on 3/11/16.
//  Copyright © 2016 Kunal Dhande. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <math.h>

#define FILENAME "input"
#define RECORD_SIZE 100
#define CHUNK_FILE_SIZE 1000000

struct hash {
    char key[10];
    char value[90];
};

long chunks;
long queueCounter;
long op_prefix;
long op_counter;
long fileToMerge;
int noOfThreads;

//Mutex Objects for each method
static pthread_mutex_t mutex_queue=PTHREAD_MUTEX_INITIALIZER;


void divideFileIntoChunks();

void mergeChuncks();
void *mergeFiles();


// Run threads to sort chunk files
void *runThreads() {
    while (1) {
        // Locking thread to increamnet operation conunter
        pthread_mutex_lock(&mutex_queue);
        ++queueCounter;
        
        // if quecounter is grater than chunk then exit from while loop
        if(queueCounter >= chunks) {
            pthread_mutex_unlock(&mutex_queue);
            break;
        }
        pthread_mutex_unlock(&mutex_queue);
        
        char program[15];
        
        // Calling external compiled program to sort chunk
        sprintf(program, "./sort %ld", queueCounter);
        system(program);
    }
    return NULL;
}



int main(int argc, const char * argv[]) {
    int i;
    noOfThreads = atoi(argv[1]);
    
    printf("\nDividing data into number of chunks");
    divideFileIntoChunks();
    
    
    printf("\nSorting data");
    queueCounter = -1;
    pthread_t *pth= malloc( noOfThreads * sizeof(pthread_t));
    for(i=0; i<noOfThreads; i++)
        pthread_create(&pth[i],NULL, runThreads, "Threads");
    
    for(i=0; i<noOfThreads; i++)
        pthread_join(pth[i], NULL);
    
    free(pth);
    
    
    printf("\nMerging sorted data");
    mergeChuncks();
    
    return 0;
}



//--------------------------------------- Divide File into number of chunks ---------------------------------------//

void divideFileIntoChunks() {
    FILE *fp = fopen(FILENAME, "r");
    char *buffer = malloc(CHUNK_FILE_SIZE * sizeof(char));
    
    if(!fp)
    return;
    
    // Calculating number of chunks
    fseek(fp, 0, SEEK_END);
    chunks = ceil(((double) ftell(fp))/CHUNK_FILE_SIZE);
    fseek(fp, 0L, SEEK_SET);
    
    printf("Chunks: %ld", chunks);
    
    long i;
    for(i=0; i<chunks; i++) {
        char str[15];
        sprintf(str, "%ld.txt", i+1);
        
        FILE *fp_t = fopen(str, "w+");
        if(!fp_t)
            continue;
        
        // Reading input file with block size 1 MB
        fread(buffer, sizeof(char), CHUNK_FILE_SIZE, fp);
        // Writing chunk file of size 1 MB
        fwrite(buffer, sizeof(char), CHUNK_FILE_SIZE, fp_t);
        
        fclose(fp_t);
    }
    fclose(fp);
    free(buffer);
}


//--------------------------------------- Merge File from number of chunks ---------------------------------------//

void mergeChuncks() {
    op_prefix = 1; // Just for namkng merged files
    op_counter = chunks; // Count for total number of files
    
    while (1) {
        
        // Renaming final output file to output.txt
        if(op_counter == 1) {
            char oldname[15];
            sprintf(oldname, "%ld.txt", op_prefix);
            rename(oldname, "output.txt");
            break;
        }
        
        op_prefix = -1 * op_prefix;
        fileToMerge = op_counter;
        op_counter = 0;
        
        queueCounter = -1;
        
        // Creating threads to merge sorted files
        pthread_t *pth= malloc( noOfThreads * sizeof(pthread_t));
        int i;
        for(i=0; i<noOfThreads; i++)
            pthread_create(&pth[i],NULL, mergeFiles, "Threads");
        
        for(i=0; i<noOfThreads; i++)
            pthread_join(pth[i], NULL);
        
        free(pth);
    }
}


void *mergeFiles() {
    while (1) {
        char *program = malloc(20 * sizeof(char));
        
         // Locking thread to increamnet operation conunter
        pthread_mutex_lock(&mutex_queue);
        ++queueCounter;
        if(queueCounter >= fileToMerge) {
            pthread_mutex_unlock(&mutex_queue);
            break;
        }
        
        // If there is signle file reamining at lat of cycle..
        if(queueCounter+1 == fileToMerge && fileToMerge%2==1) {
            pthread_mutex_unlock(&mutex_queue);
            char oldname[15];
            char newname[15];
            
            sprintf(oldname, "%ld.txt", (queueCounter+1) * (-op_prefix));
            sprintf(newname, "%ld.txt", (op_counter+1) * (op_prefix));
            rename(oldname, newname);
            op_counter++;
            continue;
            
        }
        else {
            ++queueCounter;
            // Execute external merge program
            sprintf(program, "./merge %ld %ld %ld", op_prefix, op_counter, queueCounter - 1);
            op_counter++;
            pthread_mutex_unlock(&mutex_queue);
        }
        system(program);
    }
    return NULL;
}

