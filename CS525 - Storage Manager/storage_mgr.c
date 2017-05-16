//
//  storage_mgr.c
//
//  Created by Kaushik Narayan, Kunal Dhande, Poojitha Duggappa, Hiral Ramani.
//  Copyright © 2016 Kaushik Narayan, Kunal Dhande, Poojitha Duggappa, Hiral Ramani. All rights reserved.


#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "storage_mgr.h"
#include <fcntl.h>
#include <unistd.h>
#include "dberror.h"

/*
 VERSION CONTROL

******************************************************************************************************
 *  Function Name: readBlock
 *
 *  Description:
 *      Reading a Block from the file.
 *
 *  Parameters:
 *      int pageNum : Page Number
 *      SM_FileHandle *fHandle : File Handler to the created File.
 *      SM_PageHandle memPage : Pointer to the data in a Page.
 *
 *  Return:
 *      RC : Return Code specified in the file "dberror.h".
 *
 *  Variables:
 *      FILE *fp : File Pointer which is used to open and close the created File.
 *
 *  Author:
 *      Kaushik Narayan <kbalasu6@hawk.iit.edu>
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      31-01-2016      None                                    Initialization
 *
 *      06-02-2016      <06022016 Changes>                      Added Return Codes RC_READ_NON_EXISTING_PAGE
 *                                                              and RC_OK
 *      07-02-2016      <07022016 Changes>                      Created File pointer for method and added
 *                                                              fclose() for the method.
 *      07-02-2016      <07022016 Changes>                      Changed fHandle->mgmtInfo to the created
                                                                File pointer "fp".
 *
 ******************************************************************************************************/


extern RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
//  <07022016 Changes Begin>
    
//  Opening the File with "Read" access.
    FILE *fp = fopen(fHandle->fileName,"r");
    
//  if(fHandle->mgmtInfo != NULL && (pageNum <= fHandle->totalNumPages)){
//  <07022016 Changes End>
    
//  Checking if File exists and if the given Page Number exceeds the total number of pages.
    if((fp != NULL) && (pageNum <= fHandle->totalNumPages)){
        
//      <07022016 Changes Begin>
//      fseek(fHandle->mgmtInfo, pageNum-1, SEEK_SET);
//      <07022016 Changes End>
        
//      Moving the file pointer to the given Page number.
        fseek(fp, (pageNum-1), SEEK_SET);

//      <07022016 Changes Begin>
//      fread(memPage, PAGE_SIZE , 1 , fHandle->mgmtInfo);
//      <07022016 Changes End>
        
//      Reading the content in the given Page number and moving it to memPage.
        fread(memPage, PAGE_SIZE , 1 , fp);
        
//      Setting the current page position to the given Page number.
        fHandle->curPagePos = pageNum;
        printf("\n Successfully read the Block : %d", pageNum);
        
//      <07022016 Changes Begin>
        
//      Closing the file.
        fclose(fp);
        
//      <07022016 Changes End>
//      <06022016 Changes Begin>
        
//      Returning the appropriate Return Code.
        return RC_OK;
        
//      <06022016 Changes End>
    }
    else{
        
//      Either the File does not exist or the entered Page number is more than the Total number of pages.
        printf("\n Page does not exist.");
        
//      <06022016 Changes Begin>
        
        return RC_READ_NON_EXISTING_PAGE;
        
//      <06022016 Changes End>
    }
}




/*
 VERSION CONTROL
 
 ******************************************************************************************************
 *  Function Name: getBlockPos
 *
 *  Description:
 *      Function to get the Block position.
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
 *      Kaushik Narayan <kbalasu6@hawk.iit.edu>
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      31-01-2016      None                                    Initialization
 *
 ******************************************************************************************************/


extern int getBlockPos (SM_FileHandle *fHandle)
{
//  Setting the Block position to Zero initially.
    int blockPos = 0;
    
//  Checking if the File Handler has be initiated or not.
    if(fHandle == NULL){
        printf("\n File Handler is NULL. Error. ");
    }
    else{
        
//      File Handler is present. Setting the Block position to the Current page position.
        blockPos = fHandle->curPagePos;
        printf("\n Block position is : %d",blockPos);
    }
    
//  Returning the Current Block position.
    return blockPos;
}




/*
 VERSION CONTROL
 
 ******************************************************************************************************
 *  Function Name: readFirstBlock
 *
 *  Description:
 *      Reading the First Block in the file.
 *
 *  Parameters:
 *      SM_FileHandle *fHandle : File Handler to the created File.
 *      SM_PageHandle memPage : Pointer to the data in a Page.
 *
 *  Return:
 *      RC : Return Code specified in the file "dberror.h".
 *
 *  Variables:
 *      FILE *fp : File Pointer which is used to open and close the created File.
 *
 *  Author:
 *      Kaushik Narayan <kbalasu6@hawk.iit.edu>
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      31-01-2016      None                                    Initialization
 *
 *      06-02-2016      <06022016 Changes>                      Added Return Codes RC_READ_NON_EXISTING_PAGE
 *                                                              and RC_OK
 *      07-02-2016      <07022016 Changes>                      Created File pointer and added
 *                                                              fclose() for the method.
 *      07-02-2016      <07022016 Changes>                      Changed fHandle->mgmtInfo to the created 
                                                                File pointer "fp".
 ******************************************************************************************************/

extern RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
//  <07022016 Changes Begin>
    
//  Opening the File with "Read" access.
    FILE *fp = fopen(fHandle->fileName,"r");
    
//  if(fHandle->mgmtInfo != NULL){
//  <07022016 Changes End>
    
//  Checking if the File exists or not.
    if(fp != NULL){

//      <07022016 Changes Begin>
//      fseek(fHandle->mgmtInfo, 0, SEEK_SET);
//      <07022016 Changes End>
        
//      File exists. Moving the File pointer to the First Block.
        fseek(fp, 0, SEEK_SET);

//      <07022016 Changes Begin>
//      fread(memPage, PAGE_SIZE, 1, fHandle->mgmtInfo);
//      <07022016 Changes End>

//      Reading the First Block and moving it to memPage.
        fread(memPage, PAGE_SIZE, 1, fp);
        
//      Setting the Current page position to the 1st Page.
        fHandle->curPagePos = 1;
        printf("\n First Block read successfully.");
        
//      <07022016 Changes Begin>
        
//      Closing the File.
        fclose(fp);
        
//      <07022016 Changes End>
//      <06022016 Changes Begin>
        
//      Return appropriate Return Code.
        return RC_OK;
        
//      <06022016 Changes End>
        
    }
    else{
        
//      File does not Exist. Returning appropriate Return Code.
        printf("\n Page does not exist.");
        
//      <06022016 Changes Begin>
        
        return RC_READ_NON_EXISTING_PAGE;
        
//      <06022016 Changes End>
    }
}





/*
 VERSION CONTROL
 
 ******************************************************************************************************
 *  Function Name: readPreviousBlock
 *
 *  Description:
 *      Reading the Previous Block in the file.
 *
 *  Parameters:
 *      SM_FileHandle *fHandle : File Handler to the created File.
 *      SM_PageHandle memPage : Pointer to the data in a Page.
 *
 *  Return:
 *      RC : Return Code specified in the file "dberror.h".
 *
 *  Variables:
 *      FILE *fp : File Pointer which is used to open and close the created File.
 *
 *  Author:
 *      Kaushik Narayan <kbalasu6@hawk.iit.edu>
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      31-01-2016      None                                    Initialization
 *
 *      06-02-2016      <06022016 Changes>                      Added Return Codes RC_READ_NON_EXISTING_PAGE
 *                                                              and RC_OK
 *      07-02-2016      <07022016 Changes>                      Created File pointer and added
 *                                                              fclose() for the method.
 *      07-02-2016      <07022016 Changes>                      Changed fHandle->mgmtInfo to the created
 *                                                               File pointer "fp".
 ******************************************************************************************************/

extern RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
//  <07022016 Changes Begin>
    
//  Opening File with "Read" access.
    FILE *fp = fopen(fHandle->fileName,"r");
    
//  if(fHandle->mgmtInfo != NULL){
//  <07022016 Changes End>
    
//  Checking if the File exists or not.
    if(fp != NULL){
        
//      File Exists. Now checking if the Current page Position is the First Page.
        if(fHandle->curPagePos != 1){
            
//          Current Page is not the First Page. Now checking if the Current Page is more than Zero.
            if((fHandle->curPagePos)-1 > 0){

//              <07022016 Changes Begin>
//              fseek(fHandle->mgmtInfo,((fHandle->curPagePos)-1)*PAGE_SIZE, SEEK_SET);
//              <07022016 Changes End>
                
/*              Current Page Position satisfies all conditions. Move the File pointer to the Previous
                Block from the Current Page Position. */
                fseek(fp, ((fHandle->curPagePos)-1)*PAGE_SIZE, SEEK_SET);

//              <07022016 Changes Begin>
//              fread(memPage, PAGE_SIZE, 1, fHandle->mgmtInfo);
//              <07022016 Changes End>
                
//              Reading the Previous Block and moving it to memPage.
                fread(memPage, PAGE_SIZE, 1, fp);
                
//              Change the Current Page Position to the Previous Block.
                fHandle->curPagePos = fHandle->curPagePos-1;
                printf("\n Previous block read successfully.");
                
//              <07022016 Changes Begin>
                
//              Close the File.
                fclose(fp);
                
//              <07022016 Changes End>
//              <06022016 Changes Begin>
                
                return RC_OK;
                
//              <06022016 Changes End>
            }
            else{
                
//              Block Position is Less than Zero. Return appropriate Return Code.
                printf("\n Block/File does not exist.");
                
//              <06022016 Changes Begin>
                
                return RC_READ_NON_EXISTING_PAGE;
                
//              <06022016 Changes End>
            }
        }
        else{
            
//          Block Position is the First Block. Hence no Previous Block available.
            printf("\n Previous Block unavailable since Current Block is the First Block");
            
//          <06022016 Changes Begin>
            
            return RC_READ_NON_EXISTING_PAGE;
            
//          <06022016 Changes End>
        }
    }
    else{
        
//      File does not Exist.
        printf("\n File does not Exist.");
        
//      <06022016 Changes Begin>
        
        return RC_READ_NON_EXISTING_PAGE;
        
//      <06022016 Changes End>
    }
}




/*
 VERSION CONTROL
 
 ******************************************************************************************************
 *  Function Name: readCurrentBlock
 *
 *  Description:
 *      Reading the Current Block in the file.
 *
 *  Parameters:
 *      SM_FileHandle *fHandle : File Handler to the created File.
 *      SM_PageHandle memPage : Pointer to the data in a Page.
 *
 *  Return:
 *      RC : Return Code specified in the file "dberror.h".
 *
 *  Variables:
 *      FILE *fp : File Pointer which is used to open and close the created File.
 *
 *  Author:
 *      Kaushik Narayan <kbalasu6@hawk.iit.edu>
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      31-01-2016      None                                    Initialization
 *
 *      06-02-2016      <06022016 Changes>                      Added Return Codes RC_READ_NON_EXISTING_PAGE
 *                                                              and RC_OK
 *      07-02-2016      <07022016 Changes>                      Created File pointer and added
 *                                                              fclose() for the method.
 *      07-02-2016      <07022016 Changes>                      Changed fHandle->mgmtInfo to the created
 *                                                               File pointer "fp".
 ******************************************************************************************************/


extern RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
//  <07022016 Changes Begin>
    
//  Opening the File with "Read" access.
    FILE *fp = fopen(fHandle->fileName,"r");
    
//  if(fHandle->mgmtInfo != NULL){
//  <07022016 Changes End>
    
//  Checking if the File exists or not.
    if(fp != NULL){
        
//      File Exists. Checking if the Current Page Position is valid.
        if(fHandle->curPagePos >= 0){
            
/*          Current Page Position is valid. Checking if the Current Page Position exceeds the
          Total Number of Pages. */
            if(fHandle->curPagePos <= fHandle->totalNumPages){
                
//              <07022016 Changes Begin>
//              fseek(fHandle->mgmtInfo, ((fHandle->curPagePos-1)*PAGE_SIZE), SEEK_CUR);
//              <07022016 Changes End>
                
//              All conditions satisfied. Move the File pointer to the Current Block.
                fseek(fp, ((fHandle->curPagePos-1)*PAGE_SIZE), SEEK_CUR);

//              <07022016 Changes Begin>
//              fread(memPage,PAGE_SIZE,1,fHandle->mgmtInfo);
//              <07022016 Changes End>

//              Read the Current Block and move it to memPage.
                fread(memPage,PAGE_SIZE,1,fp);
                printf("\n Current Block %d read succesfully.", fHandle->curPagePos);

//              <07022016 Changes Begin>
                
//              Close the File.
                fclose(fp);
                
//              <07022016 Changes End>
//              <06022016 Changes Begin>
                
                return RC_OK;
                
//              <06022016 Changes End>
            }
            else{
                
//              Block Positon is more than the Total Number of Pages. Return appropriate Return Code.
                printf("\nBlock position is greater than total number of Pages");
                
//              <06022016 Changes Begin>
                
                return RC_READ_NON_EXISTING_PAGE;
                
//              <06022016 Changes End>
            }
        }else{
            
//          Block Position is less than Zero a.k.a, Invalid.
            printf("\n Invalid Block position.");
            
//          <06022016 Changes Begin>
            
            return RC_READ_NON_EXISTING_PAGE;
            
//          <06022016 Changes End>
        }
    }else{
        
//      File does not Exist.
        printf("\n File does not exist.");
        
//      <06022016 Changes Begin>
        
        return RC_READ_NON_EXISTING_PAGE;
        
//      <06022016 Changes End>
    }
}





/*
 VERSION CONTROL
 
 ******************************************************************************************************
 *  Function Name: readNextBlock
 *
 *  Description:
 *      Reading the Next Block in the file.
 *
 *  Parameters:
 *      SM_FileHandle *fHandle : File Handler to the created File.
 *      SM_PageHandle memPage : Pointer to the data in a Page.
 *
 *  Return:
 *      RC : Return Code specified in the file "dberror.h".
 *
 *  Variables:
 *      FILE *fp : File Pointer which is used to open and close the created File.
 *
 *  Author:
 *      Kaushik Narayan <kbalasu6@hawk.iit.edu>
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      31-01-2016      None                                    Initialization
 *
 *      06-02-2016      <06022016 Changes>                      Added Return Codes RC_READ_NON_EXISTING_PAGE
 *                                                              and RC_OK
 *      07-02-2016      <07022016 Changes>                      Created File pointer and added
 *                                                              fclose() for the method.
 *      07-02-2016      <07022016 Changes>                      Changed fHandle->mgmtInfo to the created
 *                                                              File pointer "fp".
 ******************************************************************************************************/

extern RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
//  <07022016 Changes Begin>
    
//  Opening the File with "Read" access.
    FILE *fp = fopen(fHandle->fileName,"r");
    
//  if(fHandle->mgmtInfo!= NULL){
//  <07022016 Changes End>
    
//  Checking if File exists.
    if(fp != NULL){
        
//      File exists. Checking if Current Page Postion does not exceed Total Number of Pages.
        if((fHandle->curPagePos)+1 < fHandle->totalNumPages){
            
//          <07022016 Changes Begin>
//          fseek(fHandle->mgmtInfo, ((fHandle->curPagePos)+1)*PAGE_SIZE, SEEK_CUR);
//          <07022016 Changes End>
            
//          All conditions satisfied. Moving the File pointer to the Next Block.
            fseek(fp, ((fHandle->curPagePos)+1)*PAGE_SIZE, SEEK_CUR);
            
//          <07022016 Changes Begin>
//          fread(memPage, PAGE_SIZE, 1, fHandle->mgmtInfo);
//          <07022016 Changes End>

//          Reading the Next Block and moving it to memPage.
            fread(memPage, PAGE_SIZE, 1, fp);
            
//          Setting the Current Page Position to the Next Block by incrementing it by 1.
            fHandle->curPagePos = (fHandle->curPagePos)+1;
            printf("\n Next Block %d read succesfully.",fHandle->curPagePos);
            
//          <07022016 Changes Begin>
            
//          Closing the File.
            fclose(fp);
            
//          <07022016 Changes End>
//          <06022016 Changes Begin>
            
            return RC_OK;
            
//          <06022016 Changes End>
        }
        else{
            
//          Current Page Position is the Last Page. So the Next Block is unavailable.
            printf("\n Next Block unavailable since Current Block is the Last Block.");
            
//          <06022016 Changes Begin>
            
            return RC_READ_NON_EXISTING_PAGE;
            
//          <06022016 Changes End>
        }
    }else{
        
//      File does not Exist.
        printf("\nFile does not Exist.");
        
//      <06022016 Changes Begin>
        
        return RC_READ_NON_EXISTING_PAGE;
        
//      <06022016 Changes End>
    }
}





/*
 VERSION CONTROL
 
 ******************************************************************************************************
 *  Function Name: readLastBlock
 *
 *  Description:
 *      Reading the Last Block in the file.
 *
 *  Parameters:
 *      SM_FileHandle *fHandle : File Handler to the created File.
 *      SM_PageHandle memPage : Pointer to the data in a Page.
 *
 *  Return:
 *      RC : Return Code specified in the file "dberror.h".
 *
 *  Variables:
 *      FILE *fp : File Pointer which is used to open and close the created File.
 *
 *  Author:
 *      Kaushik Narayan <kbalasu6@hawk.iit.edu>
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      31-01-2016      None                                    Initialization
 *
 *      06-02-2016      <06022016 Changes>                      Added Return Codes RC_READ_NON_EXISTING_PAGE
 *                                                              and RC_OK
 *      07-02-2016      <07022016 Changes>                      Created File pointer and added
 *                                                              fclose() for the method.
 *      07-02-2016      <07022016 Changes>                      Changed fHandle->mgmtInfo to the created
 *                                                               File pointer "fp".
 ******************************************************************************************************/
extern RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
//  <07022016 Changes Begin>
    
//  Opening the File with "Read" access.
    FILE *fp = fopen(fHandle->fileName,"r");
    
//  if(fHandle->mgmtInfo != NULL){
//  <07022016 Changes End>
    
//  Checking if File exists.
    if(fp != NULL){
        
//      <07022016 Changes Begin>
//      fseek(fHandle->mgmtInfo, ((fHandle->totalNumPages)-1)*PAGE_SIZE, SEEK_SET);
//      <07022016 Changes End>
        
//      File exists. Moving the File pointer to the Last Block.
        fseek(fp, ((fHandle->totalNumPages)-1)*PAGE_SIZE, SEEK_SET);
      
//      <07022016 Changes Begin>
//      fread(memPage, PAGE_SIZE, 1, fHandle->mgmtInfo);
//      <07022016 Changes End>
        
//      Reading the Last Block and moving it to memPage.
        fread(memPage, PAGE_SIZE, 1, fp);
        
//      Setting the Current Page Position to the Last Page.
        fHandle->curPagePos = fHandle->totalNumPages;
        printf("\nLast Block read succesfully.");
        
//      <07022016 Changes Begin>
        
//      Closing the File.
        fclose(fp);
        
//      <07022016 Changes End>
//      <06022016 Changes Begin>
        
        return RC_OK;
        
//      <06022016 Changes End>
    }
    else{
        
//      File does not exist.
        printf("\nFile does not Exist.");
        
//      <06022016 Changes Begin>
        
        return RC_READ_NON_EXISTING_PAGE;
        
//      <06022016 Changes End>
    }
}





/*
 VERSION CONTROL
 
 ******************************************************************************************************
 *  Function Name: initStorageManager
 *
 *  Description:
 *      Initialize the Storage Manager file.
 *
 *  Parameters:
 *      Void : No parameters needed.
 *
 *  Return:
 *      Void : Returns nothing.
 *
 *  Variables:
 *      None.
 *
 *  Author:
 *      Hiral Ramani <hramani@hawk.iit.edu>
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      31-01-2016      None                                    Initialization
 *
 ******************************************************************************************************/



void initStorageManager (void){
    
}





/*
 VERSION CONTROL
 
 ******************************************************************************************************
 *  Function Name: closePageFile
 *
 *  Description:
 *      Closes the File.
 *
 *  Parameters:
 *      SM_FileHandle *fHandle : File Handler to the created File.
 *
 *  Return:
 *      RC : Return Code specified in the file "dberror.h".
 *
 *  Variables:
 *      FILE *fp : File Pointer which is used to open and close the created File.
 *
 *  Author:
 *      Hiral Ramani <hramani@hawk.iit.edu>
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      31-01-2016      None                                    Initialization
 *
 *      06-02-2016      <06022016 Changes>                      Added Return Codes RC_FILE_NOT_FOUND and
 *                                                              RC_FILE_HANDLE_NOT_INIT
 *
 *
 ******************************************************************************************************/


RC closePageFile (SM_FileHandle *fileHandle)
{
//  Opening the File with "Read" access.
    FILE *fp = fopen(fileHandle->fileName,"r");
    
//  Check if File exists.
    if(fp==NULL){
        
//      <06022016 Changes Begin>
        
//      File is not initiated.
        return RC_FILE_HANDLE_NOT_INIT;
        
//      <06022016 Changes End>
    }
    
//  Check if File is closed correctly.
    if(fclose(fp)==0){
        
//      File is closed.
        printf("The file  %s has been closed",fileHandle->fileName);
        return RC_OK;
    }
    else{
        
//      File closing met with an error.
        printf("Error is there in closing the file");
        
//      <06022016 Changes Begin>
        
        return RC_FILE_NOT_FOUND;
        
//      <06022016 Changes End>
    }
}




/*
 VERSION CONTROL
 
 ******************************************************************************************************
 *  Function Name: destroyPageFile
 *
 *  Description:
 *      Deletes the file from the memory.
 *
 *  Parameters:
 *      char *fileName : Name of the File that is created.
 *
 *  Return:
 *      RC : Return Code specified in the file "dberror.h".
 *
 *  Variables:
 *      None.
 *
 *  Author:
 *      Hiral Ramani <hramani@hawk.iit.edu>
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      31-01-2016      None                                    Initialization
 *
 ******************************************************************************************************/


RC destroyPageFile (char *fileName)
{
//  Check if file is destroyed.
    if(remove(fileName)==0){
        
//      File removed.
        return RC_OK;
    }
    else{
        
//      Unable to remove file.
        printf("Error is there in closing the file");
        return RC_FILE_NOT_FOUND;
    }
}





/*
 VERSION CONTROL
 
 ******************************************************************************************************
 *  Function Name: createPageFile
 *
 *  Description:
 *      Creates a new Page in the File with NULL.
 *
 *  Parameters:
 *      char *fileName : Name of the File to be created.
 *
 *  Return:
 *      RC : Return Code specified in the file "dberror.h".
 *
 *  Variables:
 *      FILE *fp : File Pointer which is used to open and close the created File.
 *      char buffer[PAGE_SIZE] : Character array of size PAGE_SIZE whcih stores NULL value.
 *
 *  Author:
 *      Poojitha Duggappa <pduggappa@hawk.iit.edu>
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      31-01-2016      None                                    Initialization
 *
 *      04-02-2016      <04022016 Changes>                      Added Condition to check File access.
 *
 *      07-02-2016      <07022016 Changes>                      Changed PAGE_SIZE to Zero in fseek().
 ******************************************************************************************************/

RC createPageFile(char *fileName)
{
//  Creating a File pointer.
    FILE *fp;
    
// Set a variable "buffer" to NULL.
    char buffer[PAGE_SIZE]= {'\0'};
    
//  Opening the File with "Write" access.
    fp=fopen(fileName,"w");
    
//  Check if File exists.
    if(fp != NULL){
        
//      <04022016 Changes Begin>
        
//      Check if access is "Write".
        if(access(fileName, W_OK) != -1){
            
//          <04022016 Changes End>
//          <07022016 Changes Begin>
//          fseek(fp,PAGE_SIZE,SEEK_END);
//          <07022016 Changes End>
            
//          Move File pointer to First Page.
            fseek(fp,0,SEEK_SET);
            
//          Write the NULL value to the first Page of the File.
            fwrite(buffer,1,PAGE_SIZE,fp);
        }
//      <04022016 Changes Begin>
        else{
            
//          Wrong access given.
            printf("Access not provided.");
            return RC_FILE_HANDLE_NOT_INIT;
        }
        
//          <04022016 Changes End>
    }
    else{
        
//      File does not exist.
        printf("File not found.");
        return RC_FILE_NOT_FOUND;
    }
    
//  Close the File.
    fclose(fp);
    return RC_OK;
}



/*
 VERSION CONTROL
 
 ******************************************************************************************************
 *  Function Name: openPageFile
 *
 *  Description:
 *      Opens a new Page in the created File.
 *
 *  Parameters:
 *      char *fileName : Name of the File that is created.
 *      SM_FileHandle *fHandle : File Handler to the created File.
 *
 *  Return:
 *      RC : Return Code specified in the file "dberror.h".
 *
 *  Variables:
 *      FILE *fp : File Pointer which is used to open and close the created File.
 *      int sz : Variable to store the size of the Page.
 *
 *  Author:
 *      Poojitha Duggappa <pduggappa@hawk.iit.edu>
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      31-01-2016      None                                    Initialization
 *
 *      07-02-2016      <07022016 Changes>                      Divided size by Page Size.
 *
 ******************************************************************************************************/


RC openPageFile (char *fileName,SM_FileHandle *fHandle)
{
//  Initializing size variable to Zero.
    int sz=0;
    
//  Creating a File pointer.
    FILE *fp;
    
//  Opening the File with "Read" access.
    fp=fopen(fileName,"r+");
    
//  Check if File exists.
    if(fp != NULL){
        
//      Setting the File name.
        fHandle->fileName = fileName;
        
//      Setting the Current Page Position to the the First Page.
        fHandle->curPagePos=0;

//      Moving the pointer to the Last Page.
        fseek(fp,0,SEEK_END);
        
//      Finding the size.
        sz=ftell(fp);
        
//      <07022016 Changes Begin>
//      fHandle->totalNumPages = sz;
//      <07022016 Changes End>
        
/*      Finding the actual size by dividing it by the Page Size, and setting it to the Total Number of
        Pages. */
        fHandle->totalNumPages = sz/PAGE_SIZE;
        return RC_OK;
    }
    else{
        
//      File does not exist.
        printf("File not Found.");
        return RC_FILE_NOT_FOUND;
    }
}





/******************************************************************************************************
 *  Function Name: writeCurrentBlock
 *
 *  Description:
 *      Writing blocks to the current page file
 *
 *  Parameters:
 *      int pageNum : Page Number
 *      SM_FileHandle *fHandle : File Handler to the created File.
 *      SM_PageHandle memPage : Pointer to the data in a Page.
 *
 *  Return:
 *      RC : Return Code specified in the file "dberror.h".
 *
 *  Variables:
 *      FILE *fp : File Pointer which is used to open and close the created File.
 *      int blockSize : Variable to track the size of the Block.
 *      int offsetInitial : Variable to find the Offset value.
 *
 *  Author:
 *      Kunal Dhande <kdhande@hawk.iit.edu>
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      31-01-2016      None                                    Initialization
 *
 *      05-02-2016      <05022016 Changes>                      Initial Offset value must be used instead of
 *                                                              Zero in fseek().
 *      06-02-2016      <06022016 Changes>                      File opened with access "r" instead of "rb"
 *                                                              since access given in openPageFile is "r".
 *
 ******************************************************************************************************/

RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
//  FILE pointer to access file
    FILE *fp;
    
//  To track the size of block written
    int blockSize =0;
    
//  <05022016 Changes Begin>
    
//  Finding the Offset value.
    int offsetInitial = (fHandle->totalNumPages) * PAGE_SIZE;
    printf("Offset value is %d and file name is %s and total number of pages is %d", offsetInitial, fHandle->fileName, fHandle->totalNumPages);
    
//  <05022016 Changes End>
    
//  Check if the file has atleast one page
    if(pageNum>=0){
        
//      If the pageNum value exceeds the totalNumPages, invoke ensureCapacity method
        if(pageNum>(fHandle->totalNumPages)){
            ensureCapacity(pageNum,fHandle);
        }
        
//      <06022016 Changes Begin>
//      fp = fopen(fHandle->fileName,"rb+");
//      <06022016 Changes End>
        
//      Open the file in Read mode.
        fp = fopen(fHandle->fileName,"r+");

//      <05022016 Changes Begin>
//      fseek(fp, 0, SEEK_SET);
//      <05022016 Changes End>
        
        
//      seek the file pointer to the end of the file
        fseek(fp, offsetInitial, SEEK_SET);
        
//      <05022016 Changes Begin>
        
//      Find the offset to seek to from the end of the file to the desired location to do the write operation
        offsetInitial = (pageNum - fHandle->totalNumPages) * PAGE_SIZE;
        
//      fseek(fp, 0, SEEK_END);
//      <05022016 Changes End>
        
//      Seek the file pointer to the calculated value as above from the end of file
        fseek(fp, offsetInitial, SEEK_END);
        
//      Write the file at the position pointed to by the file pointer using fwrite function
        blockSize = (int) fwrite(memPage,1,PAGE_SIZE, fp);
//      Write the data pointed to by memPage to file on disk pointed by filePointer
        
//      update the current page position
        fHandle->curPagePos=pageNum;
    }
    if(blockSize>0){
        return RC_OK;
    }
    else{
        printError(RC_WRITE_FAILED);
        return RC_WRITE_FAILED;
    }
}


/******************************************************************************************************
 *  Function Name: writeCurrentBlock
 *
 *  Description:
 *      Writing blocks to the current page file
 *
 *  Parameters:
 *      SM_FileHandle *fHandle : File Handler to the created File.
 *      SM_PageHandle memPage : Pointer to the data in a Page.
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

RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
//  call the writeBlock function by passing the current page position as the desired page number.
    return writeBlock(fHandle->curPagePos,fHandle,memPage);
}


/******************************************************************************************************
 *  Function Name: appendEmptyBlock
 *
 *  Description:
 *      Appending empty (zero byte) block to file
 *
 *  Parameters:
 *      SM_FileHandle *fHandle : File Handler to the created File.
 *
 *  Return:
 *      RC : Return Code specified in the file "dberror.h".
 *
 *  Variables:
 *      FILE *fp : File Pointer which is used to open and close the created File.
 *      char buffer[PAGE_SIZE] : Character array of size PAGE_SIZE whcih stores NULL value.
 *
 *  Author:
 *      Kunal Dhande <kdhande@hawk.iit.edu>
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      01-02-2016      None                                    Initialization
 *
 *      06-02-2016      <06022016 Changes>                      File opened with access "r" instead of "rb"
 *                                                              since access given in openPageFile is "r".
 ******************************************************************************************************/

RC appendEmptyBlock (SM_FileHandle *fHandle)
{
    printf("Appending Empty Block");
    
//  <06022016 Changes Begin>
//  FILE *fp = fopen(fHandle->fileName,"rb+");
//  <06022016 Changes End>

    FILE *fp = fopen(fHandle->fileName,"r+");
    
    char buffer[PAGE_SIZE]= {'\0'};
    
//  Go to the end of the file
    fseek(fp,(fHandle->totalNumPages)*PAGE_SIZE,SEEK_SET);
    
//  Apending empty block at the end of file
    int blockSize = (int)fwrite(buffer,1,PAGE_SIZE,fp);
    printf("The number of pages before incrementing : %d\n", fHandle->totalNumPages);
    
//  increment the total number of pages in the file
    fHandle->totalNumPages++;
    printf("The number of pages : %d\n",fHandle->totalNumPages);
    fclose(fp);
    
    if(blockSize>0)
        return RC_OK;
    else
    {
        printError(RC_WRITE_FAILED);
        return RC_WRITE_FAILED;
    }
}


/******************************************************************************************************
 *  Function Name: ensureCapacity
 *
 *  Description:
 *      If the file has less than numberOfPages pages then increase the size to numberOfPages.
 *
 *  Parameter:
 *      int numberOfPages: Total number of pages in file.
 *      SM_FileHandle *fHandle : File Handler to the created File.
 *
 *  Return:
 *      RC: Return Code
 *
 *  Variables:
 *      FILE *fp : File Pointer which is used to open and close the created File.
 *      char *buffer : Character Pointer to find the size of a new Page.
 *      int pagesToInitialise : Variable used to find the total number of Pages to add.
 *      int offset : Variable to find the Offset.
 *      int blockSize : Size of the Block for which NULL must be written.
 *
 *  Author:
 *      Kunal Dhande <kdhande@hawk.iit.edu>
 *
 *  History:
 *      Date            Search String                           Content
 *      --------------- --------------------------------------- ----------------------
 *      01-02-2016      None                                    Initialization
 *
 *      05-02-2016      <05022016 Changes>                      Initial Offset value must be used instead of
 *                                                              Zero in fseek().
 *      06-02-2016      <06022016 Changes>                      File opened with access "r" instead of "rb"
 *                                                              since access given in openPageFile is "r".
 ******************************************************************************************************/

RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle) {
    FILE *fp;
    char *buffer;
    if(fHandle->totalNumPages < numberOfPages){
        
//      <06022016 Changes Begin>
//      fp=fopen(fHandle->fileName,"rb+");
//      <06022016 Changes End>
        
        fp=fopen(fHandle->fileName,"r+");
        
//      Calculate the number of pages to increase
        int pagesToInitialise = numberOfPages - fHandle->totalNumPages;
        
        printf("The number of pages to be initialised is %d\n",pagesToInitialise);
        
//      <05022016 Changes Begin>
        
//      Calculate the offset to seek to, for increasing the number of pages
        int offset = (numberOfPages) * PAGE_SIZE;
        
//      fseek(fp,0,SEEK_SET);
//      <05022016 Changes End>
        
//      Seek to the calculated offset from the beginning of the file
        fseek(fp,offset,SEEK_SET);
        
        
//      <05022016 Changes Begin>
        
//      The offset to seek to from the end of the file in order to initialise the blocks to null
        offset = (fHandle->totalNumPages - numberOfPages)*PAGE_SIZE;

//      fseek(fp,0,SEEK_CUR);
//      <05022016 Changes End>
        
//      Seek to the calculated offset
        fseek(fp,offset,SEEK_CUR);
        
//      <05022016 Changes Begin>
        
//      Calculate the number of bytes to be written
        offset = (pagesToInitialise) * PAGE_SIZE;
        
//      allocate memory to the character pointer
        buffer = malloc(sizeof(offset));
        
//      Write null for the calculated count(offset)
        int blockSize = (int)fwrite(buffer,1,offset,fp);
        
//      <05022016 Changes End>
        
//      Update the total number of pages
        fHandle->totalNumPages = numberOfPages;
        fclose(fp);
//      free(buffer);
        return RC_OK;
    }
    else{
        printError(RC_INCREASE_CAPACITY_FAILED);
        return RC_INCREASE_CAPACITY_FAILED;
    }
}
