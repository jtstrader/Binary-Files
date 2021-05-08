LIBRARY DATABASE PROJECT

Problem: Read input from a binary file of database entries and transactions, and create new entries/make edits accordingly.
Goal: to be able to read in the input from transaction files and then send records that have been added/changed to the copyable (which will be deleted at the end of the program) while ignoring deleted records. Print errors to ERROR file accordingly

Input:
Binary database file (library.out) and binary transaction file (transact.out).

Both are organized as: ISBN|name|author|onhand|price|type
However, transact.out also has an enumator TransactionType at the beginning of every set of data

Output:
3 files:
    - update.out - binary file formatted the exact same as library.out (input) with updated records 
    - copy.out - a copy of the binary file (library.out) used for reading in records
    - ERRORS - an error file (.txt format) that shows the user any errors that occurred while going through the transactions

Algorithm:
The program will read in all the data from library.out and will assign each byte offset for the data to a map with the ISBN as the key.
After this, library.out will be closed and not opened again, as we do not want to edit the master file. The name of the master file will
be used in a system() call to create a copy as a file "copy.out". Copy.out will be used to make edits, additions, and changes.

Add: a record is added if it is not already present in the system (in copy.out). All transactions include valid ISBNs, so negatives do not need to be checked.
     when the record is added, it is sent to copy.out which will be read later to create the updated binary file.
Delete: a record is deleted if it is found in the system. To delete a record, its key is simply deleted from the map, essentially skipping over its
        byte offset when going through the copy file to create the update file.
ChangeOnhand: a record has its onhand values changed. If the record is not found an error is thrown. Otherwise, the onhand value of the transaction record
              (val from transact.out) will be compared to the value stored in copy.out. These values will be added together. If the new onhand goes under 0,
              set onhand to 0 and throw an error.
ChangePrice: a record has its price values change. If the record is not found an error is thrown. Otherwise, the price value of the transaction record
              (val from transact.out) will be compared to the value stored in copy.out. I can assume there will be no negative prices.

Error Output - 
The ERRORS file keeps track of all errors that occur in the file while going through the transactions. If any errors are hit, the ERROR file receives these errors.
ERRORS should be in .txt format for the user to read after the transaction file is read through the program.

Copy.out -
A copy of the master file to allow changes to the database. All changes are recorded into this file.

1. If a record is added to the file, its byte offset is added to the map with the new ISBN.
2. If a record is deleted, nothing happens in the copy.out file but the ISBN key is deleted.
3. If a record has its onhand value changed, the new record with the new onhand is sent to the copy.out and has its byte offset set as the new byte offset for that ISBN
4. If a record has its price value changed, the new record with the price is sent to the copy.out and has its byte offset set as the new byte offset for that ISBN

Update.out -
The update file that is created from copy.out and the map that contains all the byte offsets.

map<unsigned int, long> x; <- unsigned int is the ISBN, long is the byte offset

When creating output, the map is iterated through using a map<unsigned int, long>::iterator. The second value (iterator->second) is used to get the byte offset and seekg()
is used to move through the copy.out file. The records in copy.out that are present in the map (valid records) are sent to the update file. Any changes that were made through
ChangeOnhand and ChangePrice will be reflected in the map. After this process is done, copy.out will be deleted.

--Final Notes--

At most two book records and one transaction record can be in RAM at a time. For adding and deleting, only one record is used. For changing onhand and value prices, two
records are used: one from copy.out and one from the transaction record.

seekg/p() has been used as little as possible in the file, only present in ChangeOnhand and create update file. It was written in two times in the program and attempts were
made to minimize unecessary/time consuming I/O functions. 
