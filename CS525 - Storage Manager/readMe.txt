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
    test_assign1_1.c
    test_helper.h
    makefile

Milestone:
    31-01-2016      :   Initial coding of functions and assigning roles to Individuals.

    06-01-2016      :   Completion of Individual roles. Integration of all code.

    07-01-2016      :   Execution of Test Cases.


Installation instruction:

    1. Place all the code in local machine’s directory.

$ scp README.txt dberror.c dberror.h storage_mgr.c storage_mgr.h test_assign1_1.c test_helper.h makefile username@fourier.cs.iit.edu:~/

    2. However, if you want to add these files in a new Directory, use the following code.

$ ssh username@fourier.cs.iit.edu “mkdir ~/<Folder_Name>”
$ scp README.txt dberror.c dberror.h storage_mgr.c storage_mgr.h test_assign1_1.c test_helper.h makefile username@fourier.cs.iit.edu:~/<Folder_Name>


    3. Execute the command "make" to generate the .o files.

$ make

    4. Execute the command “./test_assign1_1” to execute the Test Cases.	

$ ./test_assign1_1



Function descriptions: of all additional functions
    None.

Additional error codes: of all additional error codes


Data structure: main data structure used


Extra credit: of all extra credits


Additional files: of all additional files


Test cases: of all additional test cases added (leave if blank if there isn't any)


Problems solved (leave if blank if there isn't any)

    1. Unable to perform any Read operation since File Pointer was not present in any Read function.

    2. Error in function Open Page File. Caused since the size was not divided by the Total Page Size. This caused the File to have Zero pages every time.

    3. Error in function Create Page File. Caused because PAGE_SIZE was used in fseek() instead of Zero. This caused Two pages to be created instead of One.


Problems to be solved (leave if blank if there isn't any)