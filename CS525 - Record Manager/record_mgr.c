#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "record_mgr.h"
#include "buffer_mgr.h"
#include "storage_mgr.h"



//Global Variables
Schema *schemaGlobal;
BM_PageHandle *pagehandle;
int slot;

//structure for parsing page
typedef struct RM_ParseHandle {
    
    //evaluation expression
    Expr* evalExp;
    //Block Position
    int blockPos;
    //Record Block
    int recBlock;
    //Block Size
    int blockSize;
}RM_ParseHandle;

/*
 VERSION CONTROL
 
 ******************************************************************************************************
 
 *   Function Name: initRecordManager
 *
 *   Description:
 *       Initiate record manager.
 *
 *   Parameters:
 *       void *mgmtData : Data
 *
 *   Return:
 *       RC_OK : Succesfull initialization.
 *
 *   Variables:
 *       pagehandle : Creates a new Page handle.
 *
 *   Author:
 *       Poojitha Vagale Duggappa <pduggappa@hawk.iit.edu>
 *
 *   History:
 *       Date            Search String                           Content
 *       --------------- --------------------------------------- ----------------------
 *       03-25-2016      None                                    Initialization
 *
 ******************************************************************************************************/


RC initRecordManager (void *mgmtData) {
    schemaGlobal = (Schema *) malloc(sizeof(Schema));
    pagehandle = MAKE_PAGE_HANDLE();
    return RC_OK;
}

/*
 VERSION CONTROL
 
 ******************************************************************************************************
 *   Function Name: shutdownRecordManager
 *
 *   Description:
 *       Function to  shutdown the recordmanager and free allocated memory
 *
 *
 *   Parameters:
 *       None
 *
 *   Return:
 *       RC_OK : Successfully released Record Manager.
 *
 *
 *   Variables:
 *       None.
 *
 *   Author:
 *       Poojitha Vagale Duggappa <pduggappa@hawk.iit.edu>
 *
 *   History:
 *       Date            Search String                           Content
 *       --------------- --------------------------------------- ----------------------
 *       03-25-2016      None                                    Initialization
 *
 ******************************************************************************************************/



RC shutdownRecordManager () {
    free(schemaGlobal);
    free(pagehandle);
    return RC_OK;
}


/*
 VERSION CONTROL
 
 ******************************************************************************************************
 *   Function Name: createTable
 *
 *   Description:
 *       Function to  shutdown the recordmanager and free allocated memory
 *
 *   Parameters:
 *       char *name : Pointer to name
 *       Schema *schema : Pointer to schema
 *
 *   Return:
 *       RC_OK : Successfull Creation.
 *
 *   Variables:
 *      RC returnFlag : Return Code.
 *      BM_BufferPool *bufferPool : Pointer for the Buffer Pool.
 *
 *   Author:
 *      Kunal Dhande<kdhande@hawk.iit.edu>
 *
 *   History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      03-29-2016      None                                    Initialization
 *
 ******************************************************************************************************/




RC createTable (char *name, Schema *schema) {
    
    //Create page file
    RC returnFlag = createPageFile(name);
    BM_BufferPool *bufferPool = MAKE_POOL();
    
    //check if page file is created
    if (returnFlag == RC_OK) {
        
        
        //initiate buffer pool
        returnFlag = initBufferPool(bufferPool, name, 3, RS_FIFO, NULL);
        
        //check if buffer pool is initiated
        if(returnFlag == RC_OK) {
            
            //pin the page
            returnFlag = pinPage(bufferPool, pagehandle, 0);
            
            //check if page is pinned
            if (returnFlag == RC_OK) {
                
                //if page is pinned, assign slot value
                int i = 0, position = 0;
                slot = 0;
                
                //temp variable for size
                char *tmp= (char *)malloc(10000000);
                pagehandle->data= tmp;
                position += sprintf(pagehandle->data+position, "numAttr: %d", schema->numAttr);
                while(schema->numAttr > i) {
                    position += sprintf(pagehandle->data+position, ", dataTypes[%d]: %d, attrNames[%d]: %s, typeLength[%d]: %d" ,i,schema->dataTypes[i], i,schema->attrNames[i], i,schema->typeLength[i]);
                    i++;
                }
                
                //once all operations are done, mark page as dirty
                returnFlag = markDirty(bufferPool, pagehandle);
                if(returnFlag == RC_OK) {
                    
                    //once page is marked dirty, unpin page
                    returnFlag = unpinPage(bufferPool, pagehandle);
                    if(returnFlag == RC_OK) {
                        
                        //if page is unpinned, flush the buffer pool
                        returnFlag = forceFlushPool(bufferPool);
                        if(returnFlag == RC_OK) {
                            
                            //if all operations work, assign schema
                            schemaGlobal = schema;
                        }
                    }
                }
            }
        }
    }
    
    return returnFlag;
}



/*
 VERSION CONTROL
 
 ******************************************************************************************************
 *   Function Name: openTable
 *
 *   Description:
 *       Function to open the created table
 *
 *   Parameters:
 *       RM_TableData *rel : Pointer to tabledata
 *       char *name : Pointer to name
 *
 *   Return:
 *       RC_OK : Successful Return Code.
 *
 *   Variables:
 *       BM_BufferPool *bufferPool : Pointer to Buffer Pool.
 *
 *   Author:
 *       Kunal Dhande<kdhande@hawk.iit.edu>
 *
 *   History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      03-29-2016      None                                    Initialization
 *
 *******************************************************************************************************/



RC openTable (RM_TableData *rel, char *name) {
    
    //create a new pool
    BM_BufferPool *bufferPool = MAKE_POOL();
    
    //initiate the buffer pool
    RC returnFlag = initBufferPool(bufferPool, name, 4, RS_FIFO, NULL);
    
    //check if buffer pool is initiated
    if(returnFlag == RC_OK) {
        
        //once initiated, assign values
        rel->name = name;
        rel->schema = schemaGlobal;
        rel->mgmtData = bufferPool;
    }
    return returnFlag;
}



/*
 VERSION CONTROL
 
 ******************************************************************************************************
 *   Function Name: closeTable
 *
 *   Description:
 *       Function to close the table
 *
 *   Parameters:
 *       RM_TableData *rel : Pointer to tabledata
 *
 *   Return:
 *       Released Schema.
 *
 *   Variables:
 *       None.
 *
 *   Author:
 *       Kunal Dhande<kdhande@hawk.iit.edu>
 *
 *   History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      03-29-2016      None                                    Initialization
 *
 *******************************************************************************************************/



RC closeTable (RM_TableData *rel) {
    schemaGlobal = rel->schema;
    return freeSchema(rel->schema);
}



/*
 VERSION CONTROL
 
 *******************************************************************************************************
 *   Function Name: deleteTable
 *
 *   Description:
 *       Function to delete the created table
 *
 *   Parameters:
 *       char *name : Pointer to name
 *
 *   Return:
 *       Deleted Page file.
 *
 *   Variables:
 *       None.
 *
 *   Author:
 *       Kunal Dhande<kdhande@hawk.iit.edu>
 *
 *   History:
 *       Date            Search String                           Content
 *       --------------- --------------------------------------- ----------------------
 *       03-29-2016      None                                    Initialization
 *
 *******************************************************************************************************/


RC deleteTable (char *name) {
    return destroyPageFile(name);
}




/* VERSION CONTROL
 
 ******************************************************************************************************
 *  Function Name: getNumTuples
 *
 *  Description:
 *      Function to get the total number of tuples from the table.
 *
 *  Parameters:
 *      RM_TableData *rel : Table data
 *
 *  Return:
 *     tuples : Number of Tuples.
 *
 *  Variables:
 *      tuples: Number of Tuples that are present.
 *		PageNumber pageNo: Number of pages.
 *      SM_FileHandle *fHandle : Handler to the FileHandle data structure.
 *      BM_BufferPool *bufferPool : Pointer to BufferPool.
 *
 *
 *  Author:
 *      Hiral Ramani <hramani@hawk.iit.edu>
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      03-25-2016      None                                    Initialization
 *
 ******************************************************************************************************/

int getNumTuples (RM_TableData *rel) {
    int tuples=0;
    PageNumber pageNo=1;
    BM_BufferPool *bufferPool = (BM_BufferPool *) rel->mgmtData;
    SM_FileHandle *fHandle = &((BufferPool*) bufferPool->mgmtData)->fHandle;
    
    //check if page number is less than total number of pages
    while(pageNo <= fHandle->totalNumPages) {
        
        //if it is, pin the existing page.
        pinPage(bufferPool, pagehandle, pageNo);
        
        //check length of page handle
        if(strlen(pagehandle->data) > 0) {
            int i;
            
            //iterate over the PAGE_SIZE
            for(i=0;i < PAGE_SIZE;i++){
                
                //check for end or page
                if(pagehandle->data[i] == '-'){
                    
                    //increment tuples if end of page is encountered.
                    tuples = tuples + 1;
                }
                else{
                    
                    //do not change the value of end of page is not encountered
                    tuples = tuples;
                }
            }
        }
        
        //unpin the page once all operations are performed
        unpinPage(bufferPool, pagehandle);
        
        //increment page number
        pageNo++;
    }
    
    //return the number of tuples found
    return tuples;
}




/* VERSION CONTROL
 
 ******************************************************************************************************
 *  Function Name: insertRecord
 *
 *  Description:
 *      Function to insert the data in table.
 *
 *  Parameters:
 *      RM_TableData *rel : Table Data.
 *		Record *record : Pointer to Record data structure.
 *
 *  Return:
 *     returnFlag : Successfull Insertion
 *
 *  Variables:
 *      BM_BufferPool *bufferPool : Pointer to Buffer Pool.
 *      SM_FileHandle *fHandle : Handler to FileHandle data structure.
 *      RID recordID : Record ID
 *      RC returnFlag : Return Code
 *
 *  Author:
 *      Kunal Dhande <kdhande@hawk.iit.edu>
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      3-29-2016      None                                    Initialization
 *
 ******************************************************************************************************/

RC insertRecord (RM_TableData *rel, Record *record) {
    BM_BufferPool *bufferPool = (BM_BufferPool *) rel->mgmtData;
    SM_FileHandle *fHandle = &((BufferPool*) bufferPool->mgmtData)->fHandle;
    RID recordID;
    
    //pin the page
    RC returnFlag = pinPage (bufferPool, pagehandle, fHandle->totalNumPages-1);
    
    //check if page is pinned
    if(returnFlag == RC_OK) {
        
        //if page is pinned, check if page number is 0 or total lenght is same as PAGE_SIZE
        if (pagehandle->pageNum==0 || strlen(pagehandle->data)==PAGE_SIZE) {
            
            //append the new record
            returnFlag = appendEmptyBlock(fHandle);
            
            //check if record is appended
            if(returnFlag == RC_OK) {
                
                //unpin the page
                returnFlag = unpinPage(bufferPool, pagehandle);
                
                //check if page is unpinned
                if(returnFlag == RC_OK) {
                    
                    //increment page number
                    pagehandle->pageNum += 1;
                    returnFlag = forcePage(bufferPool, pagehandle);
                    if(returnFlag == RC_OK) {
                        returnFlag = pinPage(bufferPool, pagehandle, fHandle->totalNumPages-1);
                    }
                }
            }
        }
        
        // if every operation works, insert data
        if(returnFlag == RC_OK) {
            
            // Append data to existing data
            strcat (pagehandle->data, record->data);
            
            // Set slot and record id of the new record
            recordID.slot = slot++;
            recordID.page = pagehandle->pageNum;
            record->id = recordID;
            
            //mark page as dirty and unpin the same
            returnFlag = markDirty(bufferPool,pagehandle);
            returnFlag = unpinPage(bufferPool,pagehandle);
            
        }
    }
    return returnFlag;
}




/* VERSION CONTROL
 
 ******************************************************************************************************
 *  Function Name: deleteRecord
 *
 *  Description:
 *      Function to delete the data in table.
 *
 *  Parameters:
 *      RM_TableData *rel : Table data.
 *		RID id : Record ID
 *
 *  Return:
 *     returnFlag : Successfull Deletion
 *
 *  Variables:
 *      BM_BufferPool *bufferPool : Pointer to Buffer Pool.
 *      RC returnFlag : Return Code
 *      char *result : value of record to be deleted
 *
 *
 *  Author:
 *      Kunal Dhande <kdhande@hawk.iit.edu>
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      03-29-2016      None                                    Initialization
 *
 ******************************************************************************************************/


RC deleteRecord (RM_TableData *rel, RID id){
    BM_BufferPool *bufferPool = (BM_BufferPool *) rel->mgmtData;
    RC returnFlag = pinPage(bufferPool, pagehandle, id.page);
    char *result;
    
    //check if page is pinned
    if(returnFlag == RC_OK) {
        
        //assign value of data to result
        result = pagehandle->data;
        //Find address of record to be deleted
        result = result + (getRecordSize(rel->schema) * id.slot==0 ? 1 : getRecordSize(rel->schema) * id.slot + 1 ) + 165;
        
        //copy the result to the data
        strcpy (pagehandle->data, result);
        
        //mark page as dirty
        returnFlag = markDirty(bufferPool, pagehandle);
        
        //check if page is marked
        if(returnFlag == RC_OK){
            
            //unpin page
            returnFlag = unpinPage(bufferPool, pagehandle);
        }
    }
    
    //return code
    return returnFlag;
}




/* VERSION CONTROL
 
 
 ******************************************************************************************************
 *  Function Name: updateRecord
 *
 *  Description:
 *      Function to update the data in table.
 *
 *  Parameters:
 *      RM_TableData *rel : Table Data
 *		Record *record : Record ID.
 *
 *  Return:
 *     returnFlag : Successfull Updation
 *
 *  Variables:
 *      BM_BufferPool *bufferPool : Pointer to Buffer Pool.
 *      RC returnFlag : Return Code
 *      char *result : value of record to be updated
 *
 *
 *  Author:
 *      Kunal Dhande <kdhande@hawk.iit.edu>
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      3-27-2016      None                                    Initialization
 *
 ******************************************************************************************************/

RC updateRecord (RM_TableData *rel, Record *record) {
    BM_BufferPool *bufferPool = (BM_BufferPool *) rel->mgmtData;
    RC returnFlag = pinPage(bufferPool, pagehandle, record->id.page);
    char *result;
    
    //check if page is pinned
    if(returnFlag == RC_OK) {
        
        //assign value of data to result
        result = pagehandle->data;
        //Find address of record to be updated
        result = result + (getRecordSize(rel->schema) * record->id.slot==0 ? 1 : getRecordSize(rel->schema) * record->id.slot + 1 ) + 165;
        
        //copy the result to the data
        strncpy(result, record->data, getRecordSize(rel->schema));// copy the contents.
        
        //mark page as dirty
        returnFlag = markDirty(bufferPool, pagehandle);
        
        if(returnFlag == RC_OK){
            
            //unpin page
            returnFlag = unpinPage(bufferPool, pagehandle);
        }

    }
    
    //return code
    return returnFlag;
}




/* VERSION CONTROL
 
 ******************************************************************************************************
 *  Function Name: getRecord
 *
 *  Description:
 *      Function to get the data from table.
 *
 *  Parameters:
 *      RM_TableData *rel : Table data.
 *      RID id : Record ID
 *		Record *record : Pointer to the Record data structure
 *
 *  Return:
 *     returnFlag : Successfull retrieval of record
 *
 *  Variables:
 *      BM_BufferPool *bufferPool : Pointer to Buffer Pool.
 *      RC returnFlag : Return Code
 *      char *result : value of record to be updated
 *
 *
 *  Author:
 *      Kunal Dhande <kdhande@hawk.iit.edu>
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      3-27-2016      None                                    Initialization
 *
 ******************************************************************************************************/


RC getRecord (RM_TableData *rel, RID id, Record *record) {
    BM_BufferPool *bufferPool = (BM_BufferPool *) rel->mgmtData;
    RC returnFlag = pinPage(bufferPool, pagehandle, id.page);
    char *result;
    
    //check if page is pinned
    if(returnFlag == RC_OK) {

        //assign value of data to result
        result = pagehandle->data;
        
        //Find address of record to be retrieved
        result = result + (getRecordSize(rel->schema) * id.slot==0 ? 1 : getRecordSize(rel->schema) * id.slot + 1 ) + 165;
        
        //copy the result to the data
        strncpy(record->data, result, getRecordSize(rel->schema));
        record->data[getRecordSize(rel->schema)] = '\0';
        record->id = id;
        
        //mark page as dirty
        returnFlag = markDirty(bufferPool, pagehandle);
        
        if(returnFlag == RC_OK){
            
            //unpin page
            returnFlag = unpinPage(bufferPool, pagehandle);
        }
    }
    
    //return code
    return returnFlag;
}


/*
 VERSION CONTROL
 
 *******************************************************************************************************
 *   Function Name: startScan
 *
 *   Description:
 *      Function to scan through the table
 *
 *   Parameters:
 *      RM_TableData *rel : Table data
 *      RM_ScanHandle *scan : Handler for ScanHandle data structure.
 *      Expr *cond : Eval Expression
 *
 *   Return:
 *      RC_OK : Successful scan.
 *      RC_START_SCAN_ERROR : Error.
 *
 *   Variables:
 *      int firstBlock : Initial block position.
 *      RM_ParseHandle* pHandle : Handler for ParseHandle data structure.
 *
 *   Author:
 *      Kaushik Narayan<kbalasu6@hawk.iit.edu>
 *
 *   History:
 *       Date            Search String                           Content
 *       --------------- --------------------------------------- ----------------------
 *       03-25-2016      None                                    Initialization
 *
 *       03-30-2016      <03302016 Changes>                      Index must be 0 not 1.
 *
 *******************************************************************************************************/


RC startScan (RM_TableData *rel, RM_ScanHandle *scan, Expr *cond){
    
    //initial block position is set to 0.
    int firstBlock = 1;
    RM_ParseHandle* pHandle=(RM_ParseHandle*)malloc(sizeof(RM_ParseHandle));
    
    //<03302016 Changes>
    //int index = 1;
    //<03302016 Changes>
    
    int index = 0;
    
    //check if handler is initiated or not
    if(pHandle!=NULL)
    {
        //Handler initiated.
        //set handler values.
        pHandle->recBlock=firstBlock;
        pHandle->blockPos=index;
        pHandle->evalExp = cond;
        scan->mgmtData=pHandle;
        scan->rel=rel; // assigning the relation
        
        //Successfully scanned and assigned values.
        return RC_OK;
    }
    else
    {
        //handler not initiated.
        //Return error message.
        return RC_START_SCAN_ERROR;
    }
    
}




/*
 VERSION CONTROL
 
 *******************************************************************************************************
 *   Function Name: next
 *
 *   Description:
 *      Function to iterate through records.
 *
 *   Parameters:
 *      Record *record : Pointer to the Record.
 *      RM_ScanHandle *scan : Handler for ScanHandle data structure.
 *
 *   Return:
 *      RC_OK : Successful scan.
 *      RC_SCAN_NEXT_ERROR : Error.
 *      RC_RM_NO_MORE_TUPLES : Error.
 *
 *   Variables:
 *      RID recordID : Record ID.
 *      int next : index of the next tuple.
 *      int count = Temporary variable.
 *      int tuples : Number of Tuples.
 *      char *tableContent : Pointer to Table Content.
 *      Value* content : Variable for the Value.
 *      RM_ParseHandle* pHandle : Handler for ParseHandle data structure.
 *      RM_TableData* tableData : Pointer to tabledata.
 *      BM_BufferPool *bufferPool : Pointer to Buffer Pool.
 *      SM_FileHandle *fHandle : Handler to FileHandle data structure.
 *      Expr* exp : Eval Expression
 *
 *   Author:
 *      Kaushik Narayan<kbalasu6@hawk.iit.edu>
 *
 *   History:
 *       Date            Search String                           Content
 *       --------------- --------------------------------------- ----------------------
 *       03-25-2016      None                                    Initialization
 *
 *******************************************************************************************************/

RC next (RM_ScanHandle *scan, Record *record) {
    
    RID recordID;
    int next = 0;
    int count = 1;
    
    char *tableContent = pagehandle->data; // character pointer table content
    Value* content;
    RM_ParseHandle* pHandle= (RM_ParseHandle*)scan->mgmtData;// Initlization of the parseHandle structure
    RM_TableData* tableData=scan->rel;
    BM_BufferPool *bufferPool = (BM_BufferPool *) tableData->mgmtData;
    SM_FileHandle *fHandle = &((BufferPool*) bufferPool->mgmtData)->fHandle;
    Expr* exp=pHandle->evalExp;
    
    int tuples = getNumTuples(scan->rel);
    // Check if Block is less than total number of pages
    while(pHandle->recBlock <= fHandle->totalNumPages){
        
        //Set current page position to the record block.
        fHandle->curPagePos = pHandle->recBlock;
        
        //pin the current page.
        if (pinPage(bufferPool, pagehandle, fHandle->curPagePos) != RC_OK)
        {
            //Return error for unsuccessul pin.
            return RC_SCAN_NEXT_ERROR;
        }
        
        //Iterate over the number of tuples.
        while(pHandle->blockPos<=tuples)
        {
            //Check if block position is not end of page
            if(tableContent[pHandle->blockPos]!='-'){
                
                //assigning values
                recordID.page=pHandle->recBlock;
                recordID.slot=pHandle->blockPos;
                
                //chec if record is retrieved.
                if (getRecord(tableData,recordID,record) != RC_OK){
                    
                    //Return error if record is not retrieved.
                    return RC_SCAN_NEXT_ERROR;
                }
                
                //check if record satisfies exal expression.
                if (evalExpr(record, tableData->schema, exp, &content)!= RC_OK){
                    
                    //Return error is it does not satisfy.
                    return RC_SCAN_NEXT_ERROR;
                }
                
                //check the value
                if(content->v.boolV == TRUE){
                    
                    //Value is bool.
                    //Set the index for the next tuple as the current count of tuples.
                    next=count;
                    
                    //set position of block to existing position + count of tuples.
                    pHandle->blockPos = pHandle->blockPos + count;
                    break;
                }
            }
            
            //if block position is end of page, directly change the block position
            pHandle->blockPos = pHandle->blockPos + count;
        }
        
        //Once all operations are performed, unpin the page.
        unpinPage(bufferPool, pagehandle);
        
        //Once all iterations are over, check if the next tuple is the same as count of tuples.
        if(next == count){
            break;
        }
        
        //assign record block to existing value + count of tuples.
        pHandle->recBlock = pHandle->recBlock + count;
    }
    
    //check if all operations are successful.
    if(next == 1){
        
        //Success
        return RC_OK;
        
    }
    else{
        
        //Return error.
        return RC_RM_NO_MORE_TUPLES; //success message
    }
    
}



/*
 VERSION CONTROL
 
 *******************************************************************************************************
 *   Function Name: closeScan
 *
 *   Description:
 *      Function to close the scan
 *
 *   Parameters:
 *      RM_ScanHandle *scan : Handler for ScanHandle data structure.
 *
 *   Return:
 *      RC_OK : Successful scan.
 *      RC_CLOSE_SCAN_ERROR : Error.
 *
 *   Variables:
 *      RM_ParseHandle* pHandle : Handler for ParseHandle data structure.
 *
 *   Author:
 *      Kaushik Narayan<kbalasu6@hawk.iit.edu>
 *
 *   History:
 *       Date            Search String                    Content
 *       --------------- -------------------------------- ----------------------
 *       03-25-2016      None                             Initialization
 *
 *       03-30-2016      <03302016 Changes>               Changed from free(scan) to free(scan->mgmtData)
 *******************************************************************************************************/

RC closeScan (RM_ScanHandle *scan) {
    
    RM_ParseHandle* pHandle=(RM_ParseHandle*)scan->mgmtData;
    
    //Check if Handler is initiated or not.
    if(pHandle!=NULL){
        
        //assign the exal expression.
        Expr* exp=pHandle->evalExp;
        
        //assign to NULL to stop memory leak.
        exp=NULL;
        
        //<03302016 Changes>
        //free(scan);
        //<03302016 Changes>
        
        //free the space of scan in ScanHandle data structure.
        free(scan->mgmtData);
        
        //stop memory leak of scan variable.
        scan->mgmtData=NULL;
        
        //return success message.
        return RC_OK;
    }
    else{
        
        //handler not initated. Return error.
        return RC_CLOSE_SCAN_ERROR;
    }
}




/*
 VERSION CONTROL
 
 *******************************************************************************************************
 *   Function Name: getRecordSize
 *
 *   Description:
 *      Fucntion to return the record size.
 *
 *   Parameters:
 *      Schema *schema : Pointer to Schema data structure.
 *
 *   Return:
 *      size : Variable that returns the size of record.
 *
 *   Variables:
 *      index : indexing variable.
 *      size : final size of the record.
 *
 *   Author:
 *      Kaushik Narayan<kbalasu6@hawk.iit.edu>
 *
 *   History:
 *       Date            Search String                    Content
 *       --------------- -------------------------------- ----------------------
 *       03-25-2016      None                             Initialization
 *
 *******************************************************************************************************/


int getRecordSize (Schema *schema) {
    
    int index = 0;
    
    //initially, assume size of record is 0.
    int size = 0;
    
    //iterate over the number of attributes present
    while(index<schema->numAttr) {
        
        //check if first value is 0
        if(schema->typeLength[index]==0) {
            
            //set the size of the record to size of integer.
            size= size+sizeof(DT_INT);
            
        }
        else{
            
            //if first value is not 0, set size to the size of the type.
            size = size + schema->typeLength[index];
        }
        
        //repeat for the next index value.
        index++;
    }
    
    //return the total size as calculate size + 3 since there are 4 values.
    return size + 3;
}




/*
 VERSION CONTROL
 
 *******************************************************************************************************
 *   Function Name: createSchema
 *
 *   Description:
 *      Function to create a specific schema.
 *
 *   Parameters:
 *      int numAttr : Number of attributes.
 *      char **attrNames : Attribute names.
 *      DataType *dataTypes : Data Types.
 *      int *typeLength : Type Length.
 *      int keySize : Key Size.
 *      int *keys : Keys.
 *
 *   Return:
 *      finalSchema : Variable that returns the size of record.
 *      NULL : Error.
 *
 *   Variables:
 *      finalSchema : Schema created in the function.
 *
 *   Author:
 *      Kaushik Narayan<kbalasu6@hawk.iit.edu>
 *
 *   History:
 *       Date            Search String                    Content
 *       --------------- -------------------------------- ----------------------
 *       03-25-2016      None                             Initialization
 *
 *******************************************************************************************************/

Schema *createSchema (int numAttr, char **attrNames, DataType *dataTypes, int *typeLength, int keySize, int *keys) {
    
    //Assign size of Created schema with the size of the actual schema.
    Schema* finalSchema=(Schema*)malloc(sizeof(Schema));
    
    //check if memory is successfully allocated
    if(finalSchema!=NULL) {
        
        //assign new values to new schema.
        finalSchema->typeLength=typeLength;
        finalSchema->numAttr=numAttr;
        finalSchema->attrNames=attrNames;
        finalSchema->keyAttrs=keys;
        finalSchema->dataTypes=dataTypes;
        finalSchema->keySize=keySize;
        
        //once all assignment is done, retun the created schema,
        return finalSchema;
    }
    else {
        
        //Return error
        return NULL;
    }
    
}




/*
 VERSION CONTROL
 
 *******************************************************************************************************
 *   Function Name: freeSchema
 *
 *   Description:
 *      Function to free the schema.
 *
 *   Parameters:
 *      Schema* schema : Pointer to the Schema data structure.
 *
 *   Return:
 *      RC_OK : Successfully released the schema.
 *
 *   Variables:
 *      None.
 *
 *   Author:
 *      Kaushik Narayan<kbalasu6@hawk.iit.edu>
 *
 *   History:
 *       Date            Search String                    Content
 *       --------------- -------------------------------- ----------------------
 *       03-25-2016      None                             Initialization
 *
 *       03-31-2016      <03312016 Changes>               Changed free(schema) to schema=NULL.
 *
 *******************************************************************************************************/

RC freeSchema(Schema* schema) {
    
    //<03312016 Changes>
    //free(schema);
    //<03312016 Changes>
    
    //set schema to null to free the memory
    schema=NULL;
    
    //return success message.
    return RC_OK;
}





/*
 VERSION CONTROL
 
 *******************************************************************************************************
 *   Function Name: createRecord
 *
 *   Description:
 *      Function to create a record.
 *
 *   Parameters:
 *      Schema* schema : Pointer to the Schema data structure.
 *      Record **record : Pointer to the Record.
 *
 *   Return:
 *      RC_OK : Successfull Creation.
 *      RC_CREATE_RECORD_FAILED : Error.
 *
 *   Variables:
 *      *record : Variable for memory allocation
 *      int size : Size of a block.
 *
 *   Author:
 *      Kaushik Narayan<kbalasu6@hawk.iit.edu>
 *
 *   History:
 *       Date            Search String                    Content
 *       --------------- -------------------------------- ----------------------
 *       03-25-2016      None                             Initialization
 *
 *******************************************************************************************************/

RC createRecord (Record **record, Schema *schema){
    
    //allocate same memory as original Record.
    (*record)=(Record*)malloc(sizeof(Record));
    int size;
    
    //check if memory is allocated
    if((*record)!=NULL) {
        
        //set the size of new record to the size of original record.
        size = getRecordSize(schema);
        
        //once size is set, allocate that size of memory to the newly created record.
        (*record)->data=(char*)malloc(sizeof(char)*size);
        
        //successfully created record.
        return RC_OK;
    }
    else{
        
        //error.
        return RC_CREATE_RECORD_FAILED;
    }
}




/*
 VERSION CONTROL
 
 *******************************************************************************************************
 *   Function Name: freeRecord
 *
 *   Description:
 *      Function to free a record.
 *
 *   Parameters:
 *      Record *record : Pointer to the Record.
 *
 *   Return:
 *      RC_OK : Successfully released the record.
 *      RC_NO_RECORD_FOUND : Error.
 *
 *   Variables:
 *      None.
 *
 *   Author:
 *      Kaushik Narayan<kbalasu6@hawk.iit.edu>
 *
 *   History:
 *       Date            Search String                    Content
 *       --------------- -------------------------------- ----------------------
 *       03-25-2016      None                             Initialization
 *
 *       03-31-2016      <03312016 Changes>               Changed free(record) to free(record->data)
 *
 *******************************************************************************************************/

RC freeRecord (Record *record) {
    
    //check if given record exists
    if (record) {
        
        //<03312016 Changes>
        //free(record);
        //<03312016 Changes>
        
        //free the record
        free(record->data);
        
        //successfully freed the record
        return RC_OK; // success message
    }
    else{
        
        //record does not exist. Error.
        return RC_NO_RECORD_FOUND;
    }
    
}



/*
 VERSION CONTROL
 
 *******************************************************************************************************
 *   Function Name: getAttr
 *
 *   Description:
 *      Function to retrieve an attribute.
 *
 *   Parameters:
 *      Record *record : Pointer to the Record.
 *      Schema *schema : Pointer to the schema data structure.
 *      int attrNum : Attribute number
 *      Value **value : Pointer to the Value
 *
 *   Return:
 *      RC_OK : Successfully released the record.
 *      RC_RM_NO_MORE_TUPLES : Error.
 *
 *   Variables:
 *      int numAttr : Number of attribures in Schema
 *      int *typeLength : Type Length
 *      Value *val : Address of Value
 *      char *text : Array with null values
 *      int next : Offset value
 *      int colLen : Length of Column
 *      int result : Final result
 *      char *temp : Temp variable
 *
 *   Author:
 *      Kaushik Narayan<kbalasu6@hawk.iit.edu>
 *
 *   History:
 *       Date            Search String                    Content
 *       --------------- -------------------------------- ----------------------
 *       03-25-2016      None                             Initialization
 *
 *
 *******************************************************************************************************/

RC getAttr(Record *record, Schema *schema, int attrNum, Value **value) {
    int numAttr = schema->numAttr;
    int *typeLength = schema->typeLength;
    Value *val = (Value*) malloc(sizeof(Value));
    char *text = malloc(100);
    text[100]='\0';
    int next = 1;
    int colLen = 0;
    int result = 0;
    char *temp=NULL;//Used to substring the data from record and convert it to required datatype.
    
    //check if the attribute number passed is less than the number of attributes
    if (attrNum < numAttr) {
        
        //Loop till the attribute is found
        for (colLen = 0; colLen < attrNum; colLen++) {
            
            //find the data type
            switch (schema->dataTypes[colLen]) {
                    case DT_INT:
                    //set offset value to size of int
                    next += sizeof(int);
                    break;
                    case DT_BOOL:
                    //set offset value to size of boolean
                    next += sizeof(bool);
                    break;
                    case DT_STRING:
                    //set offset value to typelength + column length
                    next += *(typeLength + colLen);
                    break;
                    case DT_FLOAT:
                    //set offset value to float
                    next += sizeof(float);
                    break;
            }
            
        }
        next+=attrNum;
        //add column length since the value of columnt legth would now be incremented
        int dt = *(schema->dataTypes + colLen);
        
        //switch between the data types
        switch (dt) {
                case DT_INT:
                //assign final value as int
                val->dt = DT_INT;
                result += sizeof(int);
                //allocate size needed for integer
                temp=malloc(result+1);
                break;
                
                case DT_BOOL:
                //assign final value as boolean
                val->dt = DT_BOOL;
                result += sizeof(bool);
                //allocate memory needed for boolean
                temp=malloc(result+1);
                break;
                
                case DT_FLOAT:
                //assign final value as float
                val->dt = DT_FLOAT;
                result += sizeof(float);
                //allocate size needed for float
                temp=malloc(result+1);
                break;
                
                case DT_STRING:
                //assign typelength + column length
                val->dt = DT_STRING;
                result += *(typeLength + colLen);
                //allocate memory
                temp=malloc(result+1);
                //fill up remaining values with null
                for(colLen=0;colLen <= result ; colLen++){
                    temp[colLen]='\0';
                }
                break;
        }
        
        strncpy(temp, record->data + next, result);
        temp[result]='\0';
        //convert into specific data type equivalent value
        if (val->dt == DT_INT) {
            val->v.intV = atoi(temp);
        } else if (val->dt == DT_FLOAT) {
            val->v.floatV = (float) *temp;
        } else if (val->dt == DT_BOOL) {
            val->v.boolV = (bool) *temp;
        } else if (val->dt == DT_STRING) {
            for(colLen=0;colLen < result;colLen++)
            {
                text[colLen]=temp[colLen];
            }
            text[colLen] = '\0';
            val->v.stringV=text;
        }
        
        //once all operations are completed, set the final value to the retrieved attribure
        value[0]=val;
        
        //release the temp variable
        free(temp);
        
        //successfull return code
        return RC_OK;
    }
    
    //attribute number is more than number of attributes. Error.
    return RC_RM_NO_MORE_TUPLES;
}



/*
 VERSION CONTROL
 
 *******************************************************************************************************
 *   Function Name: freeRecord
 *
 *   Description:
 *      Function to free a record.
 *
 *   Parameters:
 *      Record *record : Pointer to the Record.
 *      Schema *schema : Pointer to the schema data structure.
 *      int attrNum : Attribute number
 *      Value *value : Pointer to the Value
 *
 *   Return:
 *      RC_OK : Successfully released the record.
 *      RC_RM_NO_MORE_TUPLES : Error.
 *
 *   Variables:
 *      unsigned long numAttr : Number of attribures in Schema
 *      int *typeLength : Type Length
 *      int next : Offset value
 *      int colLen : Length of Column
 *      int *result : Final result
 *
 *   Author:
 *      Kaushik Narayan<kbalasu6@hawk.iit.edu>
 *
 *   History:
 *       Date            Search String                    Content
 *       --------------- -------------------------------- ----------------------
 *       03-25-2016      None                             Initialization
 *
 *       03-31-2016      <03312016 Changes>               Changed numAttr to unsigned long from int
 *
 *******************************************************************************************************/

RC setAttr(Record *record, Schema *schema, int attrNum, Value *value) {
    
    //<03312016 Changes>
    //int numAttr = schema->numAttr;
    //<03312016 Changes>
    
    unsigned long numAttr = schema->numAttr,temp;
    int *typeLength = schema->typeLength;
    int next = 1;
    int colLen = 0;
    char *result;
    
    //check if the attribute number passed is less than the number of attributes
    if (attrNum < numAttr) {
        
        //Loop till the attribute is found
        for (colLen = 0; colLen <attrNum; colLen++) {
            
            //find the data type
            int dt = *(schema->dataTypes + colLen);
            switch (dt) {
                    case DT_INT:
                    //set offset to size of integer
                    next += sizeof(int);
                    break;
                    case DT_BOOL:
                    //set offset to size of boolean
                    next += sizeof(bool);
                    break;
                    case DT_STRING:
                    //set offset to typelength + column length
                    next += *(typeLength + colLen);
                    break;
                    case DT_FLOAT:
                    //set offset to size of float
                    next += sizeof(float);
                    break;
            }
            
        }
        next+=attrNum;
        
        //check if attribute number is 0
        if(attrNum==0)
        {
            //set the value of result
            result =record->data ;
            
            //sepearate tuples in a page
            result[0]='-';
            
            //increment result
            result++;
        }
        else
        {
            //if attribute number is not 0
            //add offset to existing data
            result=record->data+next;
            
            //separate records in a tuple
            (result-1)[0]='~';
        }
        
        
        int dt = value->dt;
        
        //switch between the data type
        switch (dt) {
                case DT_INT:
                sprintf(result,"%d",value->v.intV);
                while(strlen(result)!=sizeof(int)) {
                    strcat(result,"0");
                }
                //convert integer to string
                for (colLen=0, numAttr=(strlen(result)-1) ; colLen < numAttr;colLen++,numAttr--) {
                    
                    //swap using the temp variable
                    temp=result[colLen];
                    result[colLen]=result[numAttr];
                    result[numAttr]=temp;
                }
                break;
                
                case DT_BOOL:
                //Convert bool to string
                sprintf(result,"%i",value->v.boolV);
                break;
                
                case DT_FLOAT:
                sprintf(result,"%f",value->v.floatV);
                while(strlen(result)!=sizeof(float)) {
                    strcat(result,"0");
                }
                //convert float to string
                for (colLen=0,numAttr=strlen(result)-1 ; colLen < numAttr;colLen++,numAttr--) {

                    //swap using temp variable
                    temp=result[colLen];
                    result[colLen]=result[numAttr];
                    result[numAttr]=temp;
                }
                break;
                
                case DT_STRING:
                //nothing to do since it is already string
                sprintf(result,"%s",value->v.stringV);
                break;
        }
        
        //success
        return RC_OK;
    }
    
    //attribute number is greater than number of attributes. Error.
    return RC_RM_NO_MORE_TUPLES;
}