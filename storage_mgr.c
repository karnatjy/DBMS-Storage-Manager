#include <stdio.h> 
#include <stdlib.h> 
#include <fcntl.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <errno.h> 
#include <string.h> 
#include <limits.h> 
#include "storage_mgr.h" 

void initStorageManager(void){
	printf("Starting the Storage Manager");   
}

RC createPageFile(char *fileName){
    FILE *file;
    char content[PAGE_SIZE];
    int write_to_file;

    file = fopen(fileName,"w");    /* creating new file */

    memset(content, '\0', sizeof(content));   /*fill the page with \0 */
    write_to_file = fwrite(content, 1, PAGE_SIZE, file);

    fclose(file);
    return RC_OK;
}

RC openPageFile (char *fileName, SM_FileHandle *fHandle){
    FILE *file;
    int size;

    file = fopen(fileName, "r");

    if(file == NULL){
        return RC_FILE_NOT_FOUND;       /* return file not found if cannot find file */
    }
    size = ftell(file); /*get file size*/

    fHandle->fileName = fileName;   /* intialize the fields of file handle with corresponding information for the file*/
    fHandle->totalNumPages = (int)(size % PAGE_SIZE + 1);
    fHandle->curPagePos = 0;

    return RC_OK;
}

RC closePageFile (SM_FileHandle *fHandle){
    fHandle->fileName = ""; /*set filename to empty and reset all values to 0*/
    fHandle->curPagePos = 0;
    fHandle->totalNumPages = 0;
    return RC_OK;
}

RC destroyPageFile (char *fileName){
    int result;
    result = remove(fileName); /*delete file and save result to result var, 0 if success else return File not found*/
    if(result == 0){
        return RC_OK;
    }else{
        return RC_FILE_NOT_FOUND;
    }
}

int getBlockPos (SM_FileHandle *fHandle)
{
	return fHandle->curPagePos; /*get curPagePos from fHandle*/
}

RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    if(pageNum>fHandle->totalNumPages-1)
        return RC_READ_NON_EXISTING_PAGE;
    else
    {
        FILE *file;
        file=fopen(fHandle->fileName,"r");
        int offset;
        offset=fHandle->curPagePos*PAGE_SIZE;
        fseek(file,offset,SEEK_SET);
        fread(memPage,sizeof(char),PAGE_SIZE,file);  /* read from file and save to memPage*/
        fHandle->curPagePos=pageNum;
        fclose(file);
        return RC_OK;
    }
}

RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	FILE *file;
	file=fopen(fHandle->fileName,"r"); /* open file */
	fseek(file,0,SEEK_SET); /* set pointer to start of file */
	fread(memPage,sizeof(char),PAGE_SIZE,file);  /*read from pointer at start of file(first block)*/
	fHandle->curPagePos=0;
	fclose(file);
	return RC_OK;
}

RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
	FILE *file; 
	file=fopen(fHandle->fileName,"r"); /* open file */
	int offset=-PAGE_SIZE; 
	fseek(file,offset,SEEK_END);    /* move pointer to end, offset is used to get the beginning of last block since the pointer is at the end*/
	fread(memPage,sizeof(char),PAGE_SIZE,file);/*read last block*/
	fHandle->curPagePos=fHandle->totalNumPages-1;
	fclose(file);
	return RC_OK;
}

RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    if(fHandle->curPagePos=0) /*if current block is 0, no previous block*/
        return RC_READ_NON_EXISTING_PAGE;
    else
    {
        FILE *file;
        file=fopen(fHandle->fileName,"r");
        int offset=(fHandle->curPagePos-1)*PAGE_SIZE; /* get off set */
        fseek(file,offset,SEEK_SET); /*pointer at beginning + offset of cur block value -1 * page size == pointer at start of previous block */
        fread(memPage,sizeof(char),PAGE_SIZE,file); /*read prev block*/
        fHandle->curPagePos=fHandle->curPagePos-1;
        fclose(file);
        return RC_OK;
    }
}
 
RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
        FILE *file;
        file=fopen(fHandle->fileName,"r");
        int offset=fHandle->curPagePos*PAGE_SIZE; /*set offset to start of current block*/
        fseek(file,offset,SEEK_SET); /*set pointer to start of cur block*/
        fread(memPage,sizeof(char),PAGE_SIZE,file); /* read from pointer*/
        fclose(file);
        return RC_OK;
}
 
RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    if(fHandle->curPagePos==fHandle->totalNumPages-1)/*check if curpagepos is last page, if it is, no more pages*/
        return RC_READ_NON_EXISTING_PAGE;
    else
    {
        FILE *file;
        file=fopen(fHandle->fileName,"r");
        int offset=(fHandle->curPagePos+1)*PAGE_SIZE; /*get offset aa start of next block*/
        fseek(file,offset,SEEK_SET);/*set pointer to start of next block*/
        fread(memPage,sizeof(char),PAGE_SIZE,file);/*read from pointer*/
        fHandle->curPagePos=fHandle->curPagePos+1;
        fclose(file);
        return RC_OK;
    }
}

RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage){
	ensureCapacity (pageNum, fHandle);	/*make sure page num is present and writable*/
	
	FILE *file;

	file=fopen(fHandle->fileName,"rb+");
	if(fseek(file,pageNum * PAGE_SIZE, SEEK_SET) != 0){
		return RC_READ_NON_EXISTING_PAGE;	
	} else if (fwrite(memPage, PAGE_SIZE, 1, file) != 1){           /*if write fails for any reasons, return write failed*/
        fclose(file);
		return RC_WRITE_FAILED; 
	} else {
		fHandle->curPagePos=pageNum;	/*if write is successful, change curpagepos*/
        fclose(file);	
		return RC_OK;
	}
}

RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
	if(fHandle == NULL){
		return RC_FILE_HANDLE_NOT_INIT;             /*return error if file not found*/
	} 

	return writeBlock(fHandle->curPagePos, fHandle, memPage); 
}

RC appendEmptyBlock (SM_FileHandle *fHandle){
	if(fHandle == NULL){
		return RC_FILE_HANDLE_NOT_INIT;
	} 

	FILE *file;
	char *space;

	space = (char *)calloc('\0', PAGE_SIZE);   /*alloc space equals to page size full of 0 */
	file=fopen(fHandle->fileName,"ab+");

	if(fwrite(space, PAGE_SIZE, 1, file) != 1)        /*if write fails for any reasons, return write failed*/ /*write space to new page (filling the new page with 0s)*/
	{
        free(space);
	    fclose(file);
		return RC_WRITE_FAILED;
	} else {
		fHandle -> totalNumPages += 1;
        free(space);
	    fclose(file);
		return RC_OK;		
	}
}

RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle){
	if(fHandle == NULL){
		return RC_FILE_HANDLE_NOT_INIT;
	} 
	if(fHandle -> totalNumPages >= numberOfPages){        /* return ok if total num pages is larger*/
		return RC_OK;
	}
	
	FILE *file;
	long extrapagespace;               
	char *space;

	extrapagespace= (numberOfPages - fHandle -> totalNumPages) * PAGE_SIZE;  /*get total number of space that the new pages will take*/
	space = (char *)calloc('\0',extrapagespace);            /*fill var space with 0*/
	
	file=fopen(fHandle->fileName,"ab+");       
    
	if(fwrite(space, extrapagespace, 1, file) == 0)   /*fill the newly created pages with 0 bytes*/
	{
        free(space);
	    fclose(file);
		return RC_WRITE_FAILED;
	} else {
		fHandle -> totalNumPages = numberOfPages;	
        free(space);
	    fclose(file);
		return RC_OK;
	}
}
 