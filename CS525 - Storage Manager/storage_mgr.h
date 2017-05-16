//
//  storage_mgr.h
//
//  Created by Kaushik Narayan, Kunal Dhande, Poojitha Duggappa, Hiral Ramani.
//  Copyright © 2016 Kaushik Narayan, Kunal Dhande, Poojitha Duggappa, Hiral Ramani. All rights reserved.

/*
 VERSION CONTROL
 
 ******************************************************************************************************
 *  File Name: storage_mgr.h
 *
 *  Description:
 *      Header file for storage_mgr.c. Contains the definitions of all functions and Data Structures.
 *
 *
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      06-02-2016      None                                    Used the "storage_mgr.h" file from the CS 525
 *                                                              repository.
 *
 ******************************************************************************************************/

#ifndef STORAGE_MGR_H
#define STORAGE_MGR_H

#include "dberror.h"

/************************************************************
 *                    handle data structures                *
 ************************************************************/
typedef struct SM_FileHandle {
    char *fileName;
    int totalNumPages;
    int curPagePos;
    void *mgmtInfo;
} SM_FileHandle;

typedef char* SM_PageHandle;

/************************************************************
 *                    interface                             *
 ************************************************************/
/* manipulating page files */
extern void initStorageManager (void);
extern RC createPageFile (char *fileName);
extern RC openPageFile (char *fileName, SM_FileHandle *fHandle);
extern RC closePageFile (SM_FileHandle *fHandle);
extern RC destroyPageFile (char *fileName);

/* reading blocks from disc */
extern RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage);
extern int getBlockPos (SM_FileHandle *fHandle);
extern RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage);
extern RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage);
extern RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage);
extern RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage);
extern RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage);

/* writing blocks to a page file */
extern RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage);
extern RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage);
extern RC appendEmptyBlock (SM_FileHandle *fHandle);
extern RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle);

#endif
