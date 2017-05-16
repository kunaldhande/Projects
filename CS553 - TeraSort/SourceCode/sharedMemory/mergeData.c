//
//  mergeData.c
//  mergeData
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

struct hash {
    char key[10];
    char value[90];
};

void mergeChuncks(long, long, long);
struct hash *getRecord(FILE *fp);
void writeToFile(FILE *fp, struct hash *record);

int main(int argc, const char * argv[]) {
    
    
    mergeChuncks(atol(argv[1]), atol(argv[2]), atol(argv[3]));
    
    return 0;
}


//--------------------------------------- Merge File from number of chunks ---------------------------------------//

void mergeChuncks(long op_prefix, long op_counter, long fileCounter) {
    char str[15];
    long recCounter1 = 0;
    long recCounter2 = 0;
    struct stat stats1, stats2;
    
    // Calculating number of records in file1
    sprintf(str, "%ld.txt", (fileCounter+1) * (-op_prefix));
    FILE *fp1 = fopen(str, "r");
    stat(str, &stats1);
    long noRec1 = stats1.st_size/RECORD_SIZE;
    noRec1 = ceil((double)noRec1/RECORD_SIZE)*RECORD_SIZE;
    
    // Calculating number of records in file2
    sprintf(str, "%ld.txt", (fileCounter+2) * (-op_prefix));
    FILE *fp2 = fopen(str, "r");
    stat(str, &stats2);
    long noRec2 = stats2.st_size/RECORD_SIZE;
    noRec2 = ceil((double)noRec2/RECORD_SIZE)*RECORD_SIZE;
    
    sprintf(str, "%ld.txt", (op_counter+1) * (op_prefix));
    FILE *fp3 = fopen(str, "w");
    
    struct hash *record1 = getRecord(fp1);
    struct hash *record2 = getRecord(fp2);
    
    
    // Compare record and append to output file accordingly
    while(1) {
        if (strcmp(record1->key, record2->key) <= 0) {
            writeToFile(fp3, record1);
            record1 = getRecord(fp1);
            recCounter1++;
            if(recCounter1 == noRec1)
                break;
        }
        else
        {
            writeToFile(fp3, record2);
            record2 = getRecord(fp2);
            recCounter2++;
            if(recCounter2 == noRec2)
                break;
        }
    }

    // Append records of file1 to output file1
    while (recCounter1 < noRec1) {
        record1 = getRecord(fp1);
        writeToFile(fp3, record1);
        recCounter1++;
    }
    free(record1);
    fclose(fp1);
    
    // Append records of file1 to output file2
    while (recCounter2 < noRec2) {
        record2 = getRecord(fp2);
        writeToFile(fp3, record2);
        recCounter2++;
    }
    fflush(fp3);
    
    
    free(record2);
    fclose(fp2);
    
    fclose(fp3);
    
    sprintf(str, "%ld.txt", (fileCounter+1) * (-op_prefix));
    remove(str);
    sprintf(str, "%ld.txt", (fileCounter+2) * (-op_prefix));
    remove(str);
}

struct hash *getRecord(FILE *fp) {
    // Read record from file
    char *str = malloc(RECORD_SIZE * sizeof(char));
    fgets(str, RECORD_SIZE, fp);
    struct hash *record = malloc(sizeof(struct hash));
    if(strcmp(str, "\n")==0)
        fgets(str, RECORD_SIZE, fp);
    
    // Making record compatible with gensort
    str[98] = '\r';
    str[99] = '\n';
    
    strncpy(record->key, str, 10); // Extract key
    strncpy(record->value, str+12, RECORD_SIZE -11); // Extract value
    
    free(str);
    
    return record;
}

void writeToFile(FILE *fp, struct hash *record) {
    // Write record to output file
    if(strlen(record->key) > 9)
        fprintf(fp, "%.10s  %s", record->key, record->value);
}
