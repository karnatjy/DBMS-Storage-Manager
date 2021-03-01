Database Theory - Programming Assignment 1 - Storage Manager

Group 19: Hung Nguyen - nguyeh6@mail.uc.edu Jayanth Reddy Karnati -
karnatjy@mail.uc.edu Thribuvan Sai Tej Dabbara - dabbartj@mail.uc.edu
Sai Aryamaan Neelagiri - neelagsn@mail.uc.edu

How to run:
-----------

\$ make test1 \$ ./test1 \$ make clean

Functions:
----------

createPageFile: --\> Used to create a page file in the memory. --\>
fopen(fileName,"w") creates an empty file for writing. If a file already
exists with the same name, it will be erased to create an empty file.
--\> File name will be as specifies in the function parameter. --\> Code
RC\_OK will be returned if no error has occured.

openPageFile: --\> The function fopen() is used to open the file and "r"
indicates read only mode. --\> Code RC\_OK will be returned if no error
has occured. Error code 'RC\_FILE\_NOT\_FOUND' will be returned if the
file cannot be found. --\> ftell() function returns the position of
read/write pointer in the file.

closePageFile: --\> This function is used to close the file. --\> Sets
file name to empty and will reset all values to zero.

destroyPageFile: --\> The remove() functions removes the file pointed by
the filename. --\> RC\_OK will be returned if the function is executed
without any errors. Else, RC\_FILE\_NOT\_FOUND will be returned.

getBlockPos: --\> Returns the current page position fron fHandle.

readBlock: --\> Checks if PageNum is valid by comparing it with
fHandle-\>totalNumPages-1. Returns RC\_READ\_NON\_EXISTING\_PAGE error
if PageNum exceeds the fHandle-\>totalNumPages-1. If no errors, else
block will be executed. --\> File will be opened in read mode, and
fseek() sets the file position of the stream to the given offset, which
is equal to fHandle-\>curPagePos\*PAGE\_SIZE. --\> File will be read and
saved to memPage. We set the current page position to PageNum and close
the file.

readFirstBlock: --\> File will be opened in read mode. --\> fseek() will
be used to set the pointer to the start of the file and file will be
read from the first block. --\> Current page position will be set to
zero and file will be closed.

readLastBlock: --\> File will be opened in read mode. --\> An offset
will be used and the pointer will be moved to the end. Offset is used to
get the beginning of last block. --\> Last block is read. Current page
position will be set to zero and file will be closed.

readPreviousBlock: --\> Checks the current page position. If it is zero,
an error RC\_READ\_NON\_EXISTING\_PAGE will be returned. Else, the
following will be executed. --\>
offset=(fHandle-\>curPagePos-1)\*PAGE\_SIZE sets the pointer at the
start of previous block.

readCurrentBlock: --\> File to be read is opened and offset is set to
the start of current block. --\> File will be read and closed. RC\_OK
will be returned.

readNextBlock: --\> We first check if the current page position is last
page, if it is, return RC\_READ\_NON\_EXISTING\_PAGE and exit. --\>
Offset will be set to the start of next block and the file will be read
from the pointer position.

writeBlock: --\> ensure page number is present and writable, else add to
it --\> write to block

writecurrentblock: --\> like writeblock but takes the current block
instead of an absolute value, a simple function call

appendemptyblock: --\> write a new block to end of file

ensurecapcity: --\> find the number of additional pages --\> append that
many pages in
