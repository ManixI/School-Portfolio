/***********************************************************************
name: Ian Manix
    readable -- recursively count readable files.
description:    
    See CS 360 Files and Directories lectures for details.
***********************************************************************/

/**********************************************************************
Take as input some path. Recursively count the files that are readable.
If a file is not readable, do not count it. Return the negative value
of any error codes you may get. However, consider what errors you want
to respond to.
**********************************************************************/

Error Codes:
    * -2  file/directory dose not exist
    * -13 no access to inital directory 

Files Test and test-2 are for testing program