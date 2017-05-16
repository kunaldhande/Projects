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
	expr.c
	expr.h
	rm_serializer.c
	record_mgr.c
	record_mgr.h
	tables.h
    test_assign3_1.c
    test_helper.h
	test_assign3_2.c
    makefile
	makefile_2

Milestone:
    03-25-2016      :   Assigning roles to Individuals.

    03-30-2016      :   Completion of Individual roles. Integration of all code.

    03-31-2016      :   Execution of Test Cases.


Installation instruction:

    1. Place all the code in local machine’s directory.

$ scp README.txt dberror.c dberror.h storage_mgr.c storage_mgr.h buffer_mgr.c buffer_mgr.h buffer_mgr_Stat.h buffer_mgr_stat.c dt.h expr.c expr.h rm_serializer.c record_mgr.c record_mgr.h tables.h test_assign3_1.c test_Assign3_2.c test_helper.h makefile makefile_2 username@fourier.cs.iit.edu:~/

    2. However, if you want to add these files in a new Directory, use the following code.

$ ssh username@fourier.cs.iit.edu “mkdir ~/<Folder_Name>”
$ scp README.txt dberror.c dberror.h storage_mgr.c storage_mgr.h buffer_mgr.c buffer_mgr.h buffer_mgr_Stat.h buffer_mgr_stat.c dt.h expr.c expr.h rm_serializer.c record_mgr.c record_mgr.h tables.h test_assign3_1.c test_Assign3_2.c test_helper.h makefile makefile_2 username@fourier.cs.iit.edu:~/<Folder_Name>


    3. Execute the command "make" to generate the .o files.

$ make

    4. Execute the command “./test_assign3_1” to execute the Test Cases.	

$ ./test_assign3_1

	5. Execute command "./test_assign3_2" to execute extra Test Case.

$ ./test_assign3_2	


Function descriptions: of all additional functions
    None.

Additional error codes: of all additional error codes

RC_NO_RECORD_FOUND 601;
RC_CREATE_RECORD_FAILED 602;
RC_CLOSE_SCAN_ERROR 603;
RC_SCAN_NEXT_ERROR 604;
RC_START_SCAN_ERROR 605;


Data structure: main data structure used

RM_ParseHandle:
    Expr* evalExp;          // Evaluation Expression
    int blockPos;			// Block Position
    int recBlock;			// Record Block
    int blockSize;			// Block Size


Extra credit: of all extra credits

1. Additional Test Cases.

Additional files: of all additional files

1. test_assign3_2.c


Test cases: of all additional test cases added (leave if blank if there isn't any)

1. test_assign3_2.c


Problems solved (leave if blank if there isn't any)


Problems to be solved (leave if blank if there isn't any)