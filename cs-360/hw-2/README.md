Dictionary Line Search Function:

This function takes a path to a dictionary [dictionaryName],
a word to search for [word], and a the length of each line [dictWidth]
This program returns the line number the searched word appears on in
the dictionary. If the word is not found it returns the nevative of
the last line searched (eg. if the last line was 10, it will return -10).

The dictionary needs to be formatted in the following way:
• Format is 1 word per line
• Lines are in ascending sorted order
• Each line is x characters long
• Use binary search

If the dictionary width provided is incorrect the results may be
inaccurate. This function will not double check this.
On error function will return 0 and set errno:

Error codes:
ERANGE - word provided is longer then dictWidth
ENOENT - the dictionary file cannot be found and/or accessed
3 - the dictionary file is empty

TODO: 
Truncate entered words to expected length
