//
//  sortData.c
//  sortData
//
//  Created by Kunal Dhande on 3/11/16.
//  Copyright © 2016 Kunal Dhande. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>


#define RECORD_SIZE 100
#define CHUNK_FILE_SIZE 1000000

struct hash {
    char key[10];
    char value[90];
};

void sortingChunkedFiles(long counter);
void merge(struct hash arr[],long min,long mid,long max, long totalNumberOfRecords);
void mergeSort(struct hash records[], long l, long r, long totalNumberOfRecords);


int main(int argc, const char * argv[]) {
    
    sortingChunkedFiles(atoi(argv[1]));
    
    return 0;
}


//---------------------------------------------- Merge Sort Algorithm ----------------------------------------------//

void sortingChunkedFiles(long counter) {
    char str[15];
    long j;
    sprintf(str, "%ld.txt", counter+1);
    
    FILE *fp_t = fopen(str, "r+");
    if(!fp_t) {
        printf("Error file reading file %s", str);
        return;
    }
    
    struct stat stats;
    stat(str, &stats);
    long no_rec = stats.st_size/RECORD_SIZE;
    no_rec = ceil((double)no_rec/RECORD_SIZE)*RECORD_SIZE;
    
    struct hash *records = malloc(no_rec* sizeof(struct hash));
    
    char *str1 = malloc(RECORD_SIZE * sizeof(char));
    for (j=0; j<no_rec; j++) {
        // get a record from chuck file
        fgets(str1, RECORD_SIZE, fp_t);
        if(strcmp(str1, "\n")==0)
            fgets(str1, RECORD_SIZE, fp_t);
        
        // Making record compatible with gensort
        str1[98] = '\r';
        str1[99] = '\n';
        
        strncpy(records[j].key, str1, 10); // Extracting key
        strncpy(records[j].value, str1+12, RECORD_SIZE -11);    // Extracting value
    }
    
    fclose(fp_t);
    
    // Call sorting fuction
    mergeSort(records, 0, no_rec - 1, no_rec);
    
    sprintf(str, "%ld.txt", counter+1);
    fp_t = fopen(str, "w");
    if(!fp_t) {
        printf("Error file reading file %s", str);
        return;
    }
    
    // Write sorted record to file
    for (j=0; j<no_rec; j++)
        fprintf(fp_t, "%.10s  %s", records[j].key, records[j].value);
    
    free(str1);
    free(records);
    
    fclose(fp_t);
}


void merge(struct hash arr[],long min,long mid,long max, long totalNumberOfRecords) {
    struct hash *tmp = malloc(totalNumberOfRecords * sizeof(struct hash));
    long i,j,k,m;
    j=min;
    m=mid+1;
    
    // Comapre two records from each set with other
    for(i=min; j<=mid && m<=max ; i++) {
        if (strcmp(arr[j].key,arr[m].key) <= 0) {
            memcpy(&tmp[i], &arr[j], RECORD_SIZE);
            j++;
        }
        else {
            memcpy(&tmp[i], &arr[m], RECORD_SIZE);
            m++;
        }
    }
    
    // Append records to sorted array from set1
    if(j>mid) {
        for(k=m; k<=max; k++) {
            memcpy(&tmp[i], &arr[k], RECORD_SIZE);
            i++;
        }
    }
    // Append records to sorted array from set2
    else {
        for(k=j; k<=mid; k++) {
            memcpy(&tmp[i], &arr[k], RECORD_SIZE);
            i++;
        }
    }
    
    // Copy again everything to original array of records
    for(k=min; k<=max; k++)
        memcpy(&arr[k], &tmp[k], RECORD_SIZE);
}



/* l is for left index and r is right index of the
 sub-array of arr to be sorted */
void mergeSort(struct hash records[], long l, long r, long totalNumberOfRecords) {
    if (l < r)  {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        long m = (l+r)/2;
        
        // Sort first and second halves
        mergeSort(records, l, m, totalNumberOfRecords);
        mergeSort(records, m+1, r, totalNumberOfRecords);
        
        merge(records, l, m, r, totalNumberOfRecords);
    }
}