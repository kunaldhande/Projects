Group Memebers:

    1.  Username : kbalasu6
        Email : kbalasu6@hawk.iit.edu
        CWID : A20344580

    2.  Username : kdhande
        Email : kdhande@hawk.iit.edu
        CWID : A20369862

    3.  Username : pduggappa
        Email : pduggappa@hawk.iit.edu
        CWID : A20370777

    4.  Username : hramani
        Email : hramani@hawk.iit.edu
        CWID : A20370004

File list:
    README.txt
    dberror.c
    dberror.h
    storage_mgr.c
    storage_mgr.h
    buffer_mgr.c
    buffer_mgr.h
    buffer_mgr_Stat.h
    buffer_mgr_stat.c
    dt.h
    test_assign2_1.c
    test_helper.h
    makefile

Milestone:
    02-24-2016      :   Initial coding of functions and assigning roles to Individuals.

    02-28-2016      :   Completion of Individual roles. Integration of all code.

    03-01-2016      :   Execution of Test Cases.


Installation instruction:

    1. Place all the code in local machine’s directory.

$ scp README.txt dberror.c dberror.h storage_mgr.c storage_mgr.h buffer_mgr.c buffer_mgr.h buffer_mgr_Stat.h buffer_mgr_stat.c dt.h test_assign2_1.c test_helper.h makefile username@fourier.cs.iit.edu:~/

    2. However, if you want to add these files in a new Directory, use the following code.

$ ssh username@fourier.cs.iit.edu “mkdir ~/<Folder_Name>”
$ scp README.txt dberror.c dberror.h storage_mgr.c storage_mgr.h buffer_mgr.c buffer_mgr.h buffer_mgr_Stat.h buffer_mgr_stat.c dt.h test_assign2_1.c test_helper.h makefile username@fourier.cs.iit.edu:~/<Folder_Name>


    3. Execute the command "make" to generate the .o files.

$ make

    4. Execute the command “./test_assign1_1” to execute the Test Cases.	

$ ./test_assign1_1



Function descriptions: of all additional functions
    None.

Additional error codes: of all additional error codes

RC_BUFFERPOOL_NULL 1001
RC_UNABLE_TO_SHUTDOWN_BUFFERPOOL 1002
RC_INVALID_STRATEGY 1003
RC_PAGE_UNPIN_FAILED 1004
RC_BUFFER_INIT_FAIL 1005


Data structure: main data structure used

Node:
    int victimCount;            // Determines page to be replaced in LRU and FIFO
    bool isDirty;               // To indicate page is dirty or not
    struct Node* nextNode;      // To point to the nextnode
    int referenceCount;         // Determines page to be replaced in LFU
    BM_PageHandle bufferPage;   // To store page contents
    int numOfAccess;            // To keep tarck of number of devices accessing the page


BufferPool:
    int maxVictimCount;     // To  store maximum vistim count number
    bool *dirtyPages;       // Points to all the dirty pages
    int readCount;          // Stores number of file read from disk to buffer after buffer manager is initialized
    Node* firstNode;        // Points to the firts node inside buffer pool
    SM_FileHandle fHandle;  // Pointer to file
    int writeCount;         // Stores number of file written back to disk from buffer after buffer manager is initialized
    PageNumber *bufferedPageContent;    // Points contents of all the buffered pages
    int *numOfAccessArr;    // contains details related to number of access of each node
    


Extra credit: of all extra credits


Additional files: of all additional files


Test cases: of all additional test cases added (leave if blank if there isn't any)


Problems solved (leave if blank if there isn't any)


Problems to be solved (leave if blank if there isn't any)