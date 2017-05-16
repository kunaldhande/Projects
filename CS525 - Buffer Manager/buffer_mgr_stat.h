/*
 VERSION CONTROL
 
 ******************************************************************************************************
 *  Function Name: buffer_mgr_stat.h
 *
 *  Description:
 *      Declarations of functions used in buffer_mgr.c.
 *
 *  Variables:
 *      None.
 *
 *  Author:
 *      Hiral Ramani, Kaushik Narayan, Kunal Dhande, Poojitha Duggappa
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *
 *
 ******************************************************************************************************/


#ifndef BUFFER_MGR_STAT_H
#define BUFFER_MGR_STAT_H

#include "buffer_mgr.h"

// debug functions
void printPoolContent (BM_BufferPool *const bm);
void printPageContent (BM_PageHandle *const page);
char *sprintPoolContent (BM_BufferPool *const bm);
char *sprintPageContent (BM_PageHandle *const page);

#endif
