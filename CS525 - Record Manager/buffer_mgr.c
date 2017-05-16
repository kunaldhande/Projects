#include<unistd.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<limits.h>
#include"dberror.h"
#include"test_helper.h"
#include"buffer_mgr.h"
#include"dt.h"
#include <pthread.h>


#define RC_BUFFERPOOL_NULL 1001
#define RC_UNABLE_TO_SHUTDOWN_BUFFERPOOL 1002
#define RC_INVALID_STRATEGY 1003
#define RC_PAGE_UNPIN_FAILED 1004
#define RC_BUFFER_INIT_FAIL 1005

//Mutex Objects for each method
static pthread_mutex_t mutex_initBufferPool=PTHREAD_MUTEX_INITIALIZER;  // Thread initializer for init Buffer Pool
static pthread_mutex_t mutex_shutdownBufferPool=PTHREAD_MUTEX_INITIALIZER; // Thread initializer for ShutDownBufferPool
static pthread_mutex_t mutex_forceFlushPool=PTHREAD_MUTEX_INITIALIZER; // Thread initializer for forceFlushPool
static pthread_mutex_t mutex_force_page=PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t work_mutex_pin=PTHREAD_MUTEX_INITIALIZER;


/// Function to inititate the Buffer manager, buffer pool and other variables
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,
                  const int numPages, ReplacementStrategy strategy, void *stratData)
{
    
    // Variable to page inside the bufferpool
    Node* node;
    // intialize buffer manager with passed values
    bm->strategy = strategy;
    bm->pageFile = (char*)pageFileName;
    bm->numPages = numPages;
    // Allocate memory for buffer pool
    BufferPool* bufferPool = (BufferPool*)malloc(sizeof(BufferPool));
    //Proceed if memory is allocated successfully
    if (bufferPool != NULL)
    {
        // First incoming thread will lock on the resource
        pthread_mutex_lock(&mutex_initBufferPool);
        
        bm->mgmtData=bufferPool;
        bufferPool->readCount=0;
        bufferPool->maxVictimCount=-1;
        bufferPool->writeCount=0;
        // To open a page file with specific access
        createPageFile(bm->pageFile);
        openPageFile(bm->pageFile,&bufferPool->fHandle);
        int i = 0;
        // Intialize all the nodes that holds pages with default values
        for(i=0;i < numPages;i++)
        {
            // Allocate memory to hold pages in bufferpool
            node=(Node*)malloc(sizeof(Node));
            node->bufferPage.data = (char *)malloc(sizeof(char)*PAGE_SIZE);
            node->bufferPage.pageNum=-1;
            node->referenceCount=0;
            node->numOfAccess=0;
            node->victimCount = -1;
            node->isDirty= FALSE;
            memset(node->bufferPage.data, 0, sizeof(char)*PAGE_SIZE);
            node->nextNode=NULL;
            Node* firstNode = bufferPool->firstNode;
            if(firstNode!=NULL)
            {
                bufferPool->firstNode = node;
                node->nextNode = firstNode;
            }
            else
            {
                bufferPool->firstNode=node;
            }
            
        }
        bufferPool->numOfAccessArr = (int*)malloc(sizeof(int)*numPages);
        bufferPool->bufferedPageContent = (PageNumber*)malloc(sizeof(PageNumber)*numPages);
        bufferPool->dirtyPages = (bool*)malloc(sizeof(bool)*numPages);
        pthread_mutex_unlock(&mutex_initBufferPool);
    }
    else
    {
        printf("/n Failed to initialize the buffer pool");
        return RC_BUFFER_INIT_FAIL;
    }
    return RC_OK;
}

// Function to  shutdown the bufferpool and free allocated memory
RC shutdownBufferPool(BM_BufferPool *const bm)
{
    // Begin only if buffer manager is initiated
    if(bm != NULL)
    {
        pthread_mutex_lock(&mutex_shutdownBufferPool);
        
        // get buffer pool from buffer manager
        BufferPool* bufferPool=(BufferPool*)bm->mgmtData;
        //get the first node or a frame inside a buffer pool
        Node* node=(Node*)bufferPool->firstNode;
        
        int i = 0;
        // Traverse through every node inside a bufferpool
        for(i=0;i < bm->numPages;i++)
        {
            // If page is not accessed anymore check,
            if(node->numOfAccess==0)
            {
                // If page is dirty
                if (node->isDirty == TRUE)
                {
                    //If page is dirty write it back to disk using forceFlushPool function
                    if(forceFlushPool(bm)==RC_WRITE_FAILED)
                    {
                        //If writing page back to disk failed
                        pthread_mutex_unlock(&mutex_shutdownBufferPool);
                        return RC_WRITE_FAILED;
                    }
                }
                
            }
            else
            {   // If even single page is still accessed then we can't shut down the buffer pool
                printf("\n unable to shutdown buffer pool due to one of the pages is still being accessed");
                pthread_mutex_unlock(&mutex_shutdownBufferPool);
                return RC_UNABLE_TO_SHUTDOWN_BUFFERPOOL;
            }
            // Increment to next code
            node = node->nextNode;
        }
        // code to free all the allocated memory
        int j=0;
        node = (Node*) bufferPool->firstNode;
        for(j=0;j<bm->numPages;j++)
        {
            free(node->bufferPage.data);
            free(node);
            node=node->nextNode;
        }
        free(bufferPool->dirtyPages);
        free(bufferPool->bufferedPageContent);
        free(bufferPool->numOfAccessArr);
        free(bufferPool);
        bufferPool = NULL;
    }
    else
    {
        printf("\n Buffer Manager not available");
        return RC_BUFFERPOOL_NULL;
    }
    pthread_mutex_unlock(&mutex_shutdownBufferPool);
    return RC_OK;
}


RC forceFlushPool(BM_BufferPool *const bm)
{
    // Proceed only if buffer manager is initiated
    if(bm != NULL)
    {
        pthread_mutex_lock(&mutex_forceFlushPool);
        //get buffer pool from buffer manager
        BufferPool* bufferPool = (BufferPool*)bm->mgmtData;
        //get first node inside buffer pool
        Node* node = bufferPool->firstNode;
        int i = 0;
        // traverse all the node and,
        for(i=0;i < bm->numPages;i++)
            // traverse all the node and,
        {
            //check page is dirty and it is nomore accessed
            if (node->isDirty == TRUE)
            {
                if(node->numOfAccess == 0){
                    // if so, write contents of file to disk
                    if(writeBlock(node->bufferPage.pageNum, &bufferPool->fHandle, (SM_PageHandle)node->bufferPage.data) == RC_FILE_NOT_FOUND){
                        pthread_mutex_unlock(&mutex_forceFlushPool);
                        return RC_WRITE_FAILED;
                    }
                    //once dirty content is written back to disk, set its dirty status to false
                    node->isDirty = FALSE;
                }
            }
            // to go to next node
            node=node->nextNode;
        }
    }
    else
    {
        return RC_BUFFERPOOL_NULL;
    }
    pthread_mutex_unlock(&mutex_forceFlushPool);
    return RC_OK;
}

/*
 VERSION CONTROL
 
 ******************************************************************************************************
 *  Function Name: markDirty
 *
 *  Description:
 *      Function marks a page as dirty
 *
 *  Parameters:
 *      BM_BufferPool *const bm     : Buffer object
 *      BM_PageHandle *const page   : Buffer Page Handler to access buffer
 *
 *  Return:
 *      RC : Return Code specified in the file "dberror.h".
 *      RC_BUFFERPOOL_NULL : Buffer Pull not is not initialized or NULL
 *
 *  Variables:
 *      None.
 *
 *  Author:
 *      Kunal Dhande <kdhande@hawk.iit.edu>
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *
 *
 ******************************************************************************************************/

RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page) {
    // proceed only if buffer manager is initialized
    if(bm) {
        // get first node from  buffer manager -> buffer pool
        BufferPool *bufferPool = ((BufferPool*)bm->mgmtData);
        Node* node= bufferPool->firstNode;
        
        int i=0;
        // loop  through all the pages inside buffer pool until requested page is found
        for(i=0;i<bm->numPages;i++) {
            // if request page is found,
            if(node->bufferPage.pageNum == page->pageNum)
            {
                // mark it as dirty
                node->isDirty=TRUE;
                return RC_OK;
            }
            // go to next node
            node = node->nextNode;
        }				
    }
    else {
        printf("\n Buffer Manager not available");
        return RC_BUFFERPOOL_NULL;
    }
    return RC_OK;
}


/*
 VERSION CONTROL
 
 ******************************************************************************************************
 *  Function Name: unpinPage
 *
 *  Description:
 *      Function unpins the page page. The pageNum field of page should be used to figure out which page to unpin.
 *
 *  Parameters:
 *      BM_BufferPool *const bm     : Buffer object
 *      BM_PageHandle *const page   : Buffer Page Handler to access buffer
 *
 *  Return:
 *      RC : Return Code specified in the file "dberror.h".
 *      RC_PAGE_UNPIN_FAILED : Page unpin operation failed
 *      RC_BUFFERPOOL_NULL : Buffer Pull not is not initialized or NULL
 *
 *  Variables:
 *      None.
 *
 *  Author:
 *      Kunal Dhande <kdhande@hawk.iit.edu>
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      31-01-2016      None                                    Initialization
 *
 ******************************************************************************************************/
RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page) {
    // proceed only if buffer manager is initialized
    if(bm) {
        // get buffer pool from buffer manager
        BufferPool * bufferPool = (BufferPool *)bm->mgmtData;
        // get first node from buffer pool
        Node * node = bufferPool->firstNode;
        
        int i;
        for(i=0;i<bm->numPages;i++) {
            //if requested page is found
            if(node->bufferPage.pageNum==page->pageNum) {
                  // check if page access count is greater than 0
                if(node->numOfAccess>0) {
                    // decrement the access count by 1
                    node->numOfAccess = node->numOfAccess-1;
                    // check if page is dirty
                    if (node->isDirty == TRUE) {
                        // if dirty, write back page contents to disk
//                        node->bufferPage.data = page->data;
                        // helps to write back dirty page contents  to disk
                        forcePage(bm, page);
                    }									
                }
                else
                    return RC_PAGE_UNPIN_FAILED;
            }
            // go to next node
            node = node->nextNode;
        }
        return RC_OK;		
    }
    else
        return RC_BUFFERPOOL_NULL;
}


/*
 VERSION CONTROL
 
 ******************************************************************************************************
 *  Function Name: forcePage
 *
 *  Description:
 *      Function should write the current content of the page back to the page file on disk.
 *
 *  Parameters:
 *      BM_BufferPool *const bm     : Buffer object
 *      BM_PageHandle *const page   : Buffer Page Handler to access buffer
 *
 *  Return:
 *      RC : Return Code specified in the file "dberror.h".
 *      RC_BUFFERPOOL_NULL : Buffer Pull not is not initialized or NULL
 *
 *  Variables:
 *      None.
 *
 *  Author:
 *      Kunal Dhande <kdhande@hawk.iit.edu>
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      31-01-2016      None                                    Initialization
 *
 ******************************************************************************************************/

RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page) {
    // proceed only if buffer manager is initialized
    if(bm) {
        pthread_mutex_lock(&mutex_force_page);
        // get buffer pool from buffer manager
        BufferPool* bufferPool = (BufferPool*)bm->mgmtData;
        
        // write dirty page contents back to disk
        writeBlock(page->pageNum, &bufferPool->fHandle, (SM_PageHandle)page->data);
        // increment write count by 1
        bufferPool->writeCount = bufferPool->writeCount+1;
        pthread_mutex_unlock(&mutex_force_page);
        return RC_OK;
    }
    else
        return RC_BUFFERPOOL_NULL;
}



/*
 VERSION CONTROL
 
 ******************************************************************************************************
 *  Function Name: pinPage
 *
 *  Description:
 *
 *
 *  Parameters:
 *      SM_FileHandle *fHandle : File Handler to the created File.
 *
 *  Return:
 *      RC : Return Code specified in the file "dberror.h".
 *
 *  Variables:
 *      None.
 *
 *  Author:
 *      Kunal Dhande <kdhande@hawk.iit.edu>
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      31-01-2016      None                                    Initialization
 *
 ******************************************************************************************************/
RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum) {

    // proceed only if buffer manager is initialized
    if(bm)
    {
        char *pageData = malloc(sizeof(char) * 512);
        sprintf(pageData, "%s-%i", "Page", pageNum);
        
        pthread_mutex_lock(&work_mutex_pin);
        
        // get buffer pool from buffer manager
        BufferPool * bufferPool = (BufferPool *)bm->mgmtData;
        // get first node from buffer pool
        Node * node = bufferPool->firstNode;
        
        int i;
        for(i=0;i<bm->numPages;i++) {
            // if request page is found
            if (node->bufferPage.pageNum == pageNum)
            {
                // get contents of page
//                page->data = node->bufferPage.data;
                page->pageNum = pageNum;
                // increment access count by one
                node->numOfAccess=node->numOfAccess+1;
                // check what is the strategy buffer manager initiated with
                switch (bm->strategy) {
                    case RS_FIFO:
                                    // For fifo nothing is required
                                    break;
                        
                    case RS_LRU:
                                    // In LRU, node with less victim count is least recently used and get replaced first
                                    node->victimCount = bufferPool->maxVictimCount + 1;
                                    // update max victim count with current node's victim  count
                                    bufferPool->maxVictimCount = node->victimCount;
                                    break;
                        
                    default:
                        pthread_mutex_unlock(&work_mutex_pin);
                        return RC_INVALID_STRATEGY;
                }
                pthread_mutex_unlock(&work_mutex_pin);
                return RC_OK;
            }
            node = node->nextNode;
        }
        
        // If requested page is not found inside buffer pool
        
        // get first node from buffer pool
        node=bufferPool->firstNode;
        // assign some big numbers
        int leastVictimCount=SHRT_MAX;
        int leastReferenceCount=SHRT_MAX;
        // pointer to store node reference that will be replaced
        Node* leastRecentlyUsedNode = NULL;
        
        for(i=0;i<bm->numPages;i++) {
            switch (bm->strategy) {
            
                    // if strategy is FIFO or LRU, find page with least victim count to replace
                    case RS_FIFO:
                    case RS_LRU:
                                // FIFO will have victim count in the order of page came into the buffer, i.e. first page came into buffer will have less victim count when compare to second one.
                                // LRU, will have victim count based number of hits and how recently they got accessed
                                if(node->victimCount<leastVictimCount && node->numOfAccess==0) {
                                    leastVictimCount = node->victimCount;
                                    leastRecentlyUsedNode = node;
                                }
                                break;
                    default:
                                pthread_mutex_unlock(&work_mutex_pin);
                                return RC_INVALID_STRATEGY;
            }
            // go to next node
            node=node->nextNode;
        }
        
        //  once node that can be replaced is found
        node=leastRecentlyUsedNode;
        
        // replace old page content with new page content
        readBlock(pageNum,&bufferPool->fHandle,(SM_PageHandle)node->bufferPage.data);
        bufferPool->readCount = bufferPool->readCount+1;
        page->pageNum=pageNum;
        node->bufferPage.pageNum = pageNum;
        node->numOfAccess=node->numOfAccess+1;
//        page->data=node->bufferPage.data;
        node->referenceCount = 1;
        node->victimCount = bufferPool->maxVictimCount + 1;
        bufferPool->maxVictimCount = node->victimCount;
        pthread_mutex_unlock(&work_mutex_pin);
        return RC_OK;
    }
    else
        return RC_BUFFERPOOL_NULL;
}


PageNumber *getFrameContents(BM_BufferPool * const bm)
{
    BufferPool* bufferPool = (BufferPool*)bm->mgmtData;
    PageNumber* frameContents = malloc(bm->numPages * sizeof(PageNumber));
    
    Node *node = bufferPool->firstNode;
    int i=0;
    for(i=0;i<bm->numPages;i++) {
        frameContents[i]= (node->bufferPage.pageNum != NO_PAGE) ? node->bufferPage.pageNum : -1;
        node = node->nextNode;
    }
    return frameContents;
}

bool *getDirtyFlags (BM_BufferPool *const bm) {
    BufferPool* bufferPool = (BufferPool*)bm->mgmtData;
    bool *dirtyFlags = malloc(bm->numPages * sizeof(bool));
    
    Node *node = bufferPool->firstNode;
    int i;
    for(i=0;i<bm->numPages;i++) {
        dirtyFlags[i]= (node->isDirty == TRUE) ? TRUE : FALSE;
        node = node->nextNode;
    }
    return dirtyFlags;
}

int *getFixCounts (BM_BufferPool *const bm) {
    BufferPool* bufferPool = (BufferPool*)bm->mgmtData;
    int *fixCounts = calloc(bm->numPages, sizeof(int));
    
    Node *node = bufferPool->firstNode;
    int i;
    for(i=0;i<bm->numPages;i++) {
        fixCounts[i] = node->numOfAccess;
        node = node->nextNode;
    }
    return fixCounts;
}


int getNumReadIO(BM_BufferPool * const bm)
{
    BufferPool* bufferPool = (BufferPool*)bm->mgmtData;
    return bufferPool->readCount;
}


int getNumWriteIO(BM_BufferPool * const bm)
{
    BufferPool* bufferPool = (BufferPool*)bm->mgmtData;
    return bufferPool->writeCount;
}

