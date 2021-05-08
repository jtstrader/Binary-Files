#include<cstdlib>
#include<iostream>
#include<fstream>
#include<sstream>
#include<climits>
#include<iomanip>
#include<map>
#include<sstream>

using namespace std;

enum TransactionType {Add, Delete, ChangeOnhand, ChangePrice};

typedef char String[25];
struct BookRec {
    unsigned int isbn;
        // err 1: isbn less than 1. write error message to cerr and IGNORE record
        // err 2: isbn is less than or equal to the previous isbn. write error message to cerr and write record to cout. add record to output file
    String name;
    String author;
    int onhand;
    float price;
    String type;
};

struct TransactionRec {
    TransactionType ToDo;
    BookRec B;
};

struct BookRec;
struct TransactionRec;

// argument checks and initalizers
bool checkArgs(int argc, char** argv); // checks arguments and attempts to open all files
long buildMapMaster(char* fileName, map<unsigned int, long>& byteOffsetM); // reads in data from master file and indexes them in a map

// transactions & file manipulations //////////////////////////////////////////////
void completeTransactions(char** files, map<unsigned int, long>& byteOffsetM, long& offset); // completes all provided transactions

void addRecord(TransactionRec* tRec, map<unsigned int, long>& byteOffsetM, fstream& outFile, fstream& ERRORS, int& transactionNum, long& offset); // adds a given record
void deleteRecord(TransactionRec* tRec, map<unsigned int, long>& byteOffsetM, fstream& ERRORS, int& transactionNum); // deletes a given record
void changeOnhand(TransactionRec* tRec, map<unsigned int, long>& byteOffsetM, fstream& outFile, fstream& ERRORS, int& transactionNum, long& offset); // changes onhand of a given record
void changePrice(TransactionRec* tRec, map<unsigned int, long>& byteOffsetM, fstream& outFile, fstream& ERRORS, int& transactionNum, long& offset); // changes price of a given record

void createUpdateFile(char* fileName, fstream& outFile, map<unsigned int, long>& byteOffsetM); // creates update file post-transactions
void createCopyFile(char* fileName); // adds copy file using system();
void deleteCopyFile(); // deletes copy file (name assumed to be "copy.out");
///////////////////////////////////////////////////////////////////////////////////

// printing & formatting
void printFile(char* fileName); // prints entire file of records
void printRecord(BookRec record); // prints single record
string keyFormat(unsigned int& key); // formats key to have 0 in front if less than 10 digits 
string toString(unsigned int& key);  // converts key to string

int main(int argc, char** argv) {
    system("clear");
    // check args
    if(!checkArgs(argc, argv))
        return 1;
    // create and populate map with byte offsets from binary file
    map<unsigned int, long> byteOffsetM;
    long offset = buildMapMaster(argv[1], byteOffsetM);
    completeTransactions(argv, byteOffsetM, offset);

    return 0;
}

// check arguments
bool checkArgs(int argc, char** argv) {
    // specs:
    //  - ./a.out master_file transaction_file new_master
    //  - argc MUST be 4
    //  - new_master name does not need to be defined as long as it is not
    //    the same as transaction_file or new_master
    //  - master_file cannot be overwritten/edited. all edits go to new_master
    
    // failure var
    bool failVar = true;

    // check argc
    if(argc!=4) {
        cerr<<"\t> ERROR: Invalid number of arguments. Please use command as follows: "<<endl;
        cout<<"\t> ./a.out master_file transaction_file new_master"<<endl;
        return false;
    }
    
    // attempt to open master file
    ifstream master(argv[1]);
    if(!master.is_open()) {
        master.close();
        cerr<<"\t> ERROR: Invalid name of master file: file '"<<argv[1]<<"' not found."<<endl;
        failVar = false;
    }
    master.close();

    // attempt to open transaction file
    ifstream transaction(argv[2]);
    if(!transaction.is_open()) {
        transaction.close();
        cerr<<"\t> ERROR: Invalid name of transaction file: file '"<<argv[2]<<"' not found."<<endl;
        failVar = false;
    }
    transaction.close();
    
    // make sure new master does not equal master or transaction
    string m = argv[1]; // master
    string t = argv[2]; // transaction
    string n = argv[3]; // new master

    if(n==m) {
        cerr<<"\t> ERROR: New master file has the same name as the current master file: '"<<t<<"'."<<endl;
        failVar = false;
    }
    if(n==t) {
        cerr<<"\t> ERROR: New master file has the same name as the current transaction file: '"<<t<<"'."<<endl;
        failVar = false;
    }
    if(m==t) {
        cerr<<"\t> ERROR: Master file cannot have the same name as the transaction file: '"<<m<<"'."<<endl;
        failVar = false;
    }

    return failVar;
}


// builds map from master input binary file
long buildMapMaster(char* fileName, map<unsigned int, long>& byteOffsetM) {
    fstream binaryFile(fileName, ios::in|ios::binary); // binary file input
    BookRec* record = new BookRec; long offset = 0;
    while(binaryFile.read((char*) record, sizeof(BookRec))) // read in data
        byteOffsetM[record->isbn] = sizeof(BookRec) * (offset++); // isbn key contains byte offset
    binaryFile.close();
    return offset;
}

// complete transactions
void completeTransactions(char** files, map<unsigned int, long>& byteOffsetM, long& offset) {

    //                        0      1          2            3
    // enum TransactionType {Add, Delete, ChangeOnhand, ChangePrice};

    // logic:
    /* 1. Create copy of master file to add items to for the creation of the output file
     * 2. Read in data from transact.out, and use enum type to select which option will be used
     * 3. 4 possible options:
     *    a. add record: append record to end of copy file with all data IF not a duplicate. use iterator to search through map and find duplicates
     *    b. delete record: remove record from byteOffsetM map, as byteOffsetM map will be used to create the output file. 
     *    c. change onhand: add onhand val from TransactionRec to record stored at byteOffsetM, append to file, and change ISBN of previous entry to this new entry
     *    d. change price: change onhand process but with the price vars
    */

    // files
    createCopyFile(files[1]); // create copy file (master file name required)
    fstream inFile(files[2], ios::in|ios::binary); // transaction binary
    fstream outFile("copy.out", ios::in|ios::out|ios::binary|ios::app); // copy binary
    fstream ERRORS("ERRORS", ios::out); // error file

    TransactionRec* tRec = new TransactionRec; int transactionNum = 1;
    while(inFile.read((char*) tRec, sizeof(TransactionRec))) {
        // 0 - add record
        // 1 - delete record
        // 2 - change onhand
        // 3 - change price
        
        switch(tRec->ToDo) {
            case Add: addRecord(tRec, byteOffsetM, outFile, ERRORS, transactionNum, offset); break;
            case Delete: deleteRecord(tRec, byteOffsetM, ERRORS, transactionNum); break;
            case ChangeOnhand: changeOnhand(tRec, byteOffsetM, outFile, ERRORS, transactionNum, offset); break;
            case ChangePrice: changePrice(tRec, byteOffsetM, outFile, ERRORS, transactionNum, offset); break;
        }
        transactionNum++;
    }

    // write to new output
    createUpdateFile(files[3], outFile, byteOffsetM);
    printFile(files[3]);

    // file closing
    inFile.close();
    outFile.close();
    ERRORS.close();
    deleteCopyFile();
}

// adds a record by given ISBN in tRec
void addRecord(TransactionRec* tRec, map<unsigned int, long>& byteOffsetM, fstream& outFile, fstream& ERRORS, int& transactionNum, long& offset) {
    // ISBN already exists
    if(byteOffsetM.find(tRec->B.isbn)!=byteOffsetM.end()) {
        // ignore record and write error to error file
        ERRORS<<"Error in transaction number   "<<transactionNum<<": cannot add--duplicate key "<<keyFormat(tRec->B.isbn)<<endl;
    }
    // Add new record to copy file
    else {
        byteOffsetM[tRec->B.isbn] = sizeof(BookRec) * (offset++); // save offset of added var
        outFile.write((char*) &(tRec->B), sizeof(BookRec)); // append to file
    }
}

// deletes a record by given ISBN in tRec
void deleteRecord(TransactionRec* tRec, map<unsigned int, long>& byteOffsetM, fstream& ERRORS, int& transactionNum) {
    // ISBN not found: no item can be deleted
    if(byteOffsetM.find(tRec->B.isbn)==byteOffsetM.end()) {
        // ignore transaction and write error to error file
        ERRORS<<"Error in transaction number   "<<transactionNum<<": cannot delete--no record found for key "<<keyFormat(tRec->B.isbn)<<endl;
    }
    // Delete record by removing information from map
    else {
        // map::erase at iterator object is found at
        byteOffsetM.erase(byteOffsetM.find(tRec->B.isbn));
    }
}

// changes onhand var of record by given ISBN in tRec
void changeOnhand(TransactionRec* tRec, map<unsigned int, long>& byteOffsetM, fstream& outFile, fstream& ERRORS, int& transactionNum, long& offset) {
    // ISBN not found: no item can be changed
    if(byteOffsetM.find(tRec->B.isbn)==byteOffsetM.end()) {
        // ignore transaction and write error to error file
        ERRORS<<"Error in transaction number   "<<transactionNum<<": cannot change count--no such key "<<keyFormat(tRec->B.isbn)<<endl;
    }
    else {
        // grab BookRec from copy.out and add onhand change. throw error if onhand<0
        long recordBytePos = byteOffsetM[tRec->B.isbn]; // grab byte position of requested record
        outFile.seekg(recordBytePos, ios::beg); // position needle at requested record
        BookRec record;
        outFile.read((char*) &record, sizeof(BookRec)); // read in new book record from outfile
        tRec->B.onhand+=record.onhand; // add B.onhand to tRec->B
        
        // set onhand to 0 if underflow
        if(tRec->B.onhand<0) {
            // error message
            ERRORS<<"Error in transaction number   "<<transactionNum<<": count = "<<tRec->B.onhand<<" for key "<<keyFormat(tRec->B.isbn)<<endl;
            tRec->B.onhand = 0; // set onhand to zero if underflow
        }
        // write to file
        byteOffsetM[tRec->B.isbn] = sizeof(BookRec) * (offset++); // save offset of added var
        outFile.write((char*) &(tRec->B), sizeof(BookRec)); // append to file
    }
}

void changePrice(TransactionRec* tRec, map<unsigned int, long>& byteOffsetM, fstream& outFile, fstream& ERRORS, int& transactionNum, long& offset) {
    // ISBN not found: no item can be changed
    if(byteOffsetM.find(tRec->B.isbn)==byteOffsetM.end()) {
        // ignore transaction and write error to error file
        ERRORS<<"Error in transaction number   "<<transactionNum<<": cannot change price--no such key "<<keyFormat(tRec->B.isbn)<<endl;
    }
    // write to file
    else {
        byteOffsetM[tRec->B.isbn] = sizeof(BookRec) * (offset++); // save offset of added var
        outFile.write((char*) &(tRec->B), sizeof(BookRec)); // append to file
    }
}

// create update file from map
void createUpdateFile(char* fileName, fstream& outFile, map<unsigned int, long>& byteOffsetM) {
    fstream update(fileName, ios::out|ios::binary); // create update file
    for(map<unsigned int, long>::iterator it = byteOffsetM.begin(); it!=byteOffsetM.end(); ++it) {
        BookRec record;
        outFile.seekg(it->second); // move needle to requested record from byteoffset map
        outFile.read((char*) &record, sizeof(BookRec)); // read in record
        update.write((char*) &record, sizeof(BookRec)); // add record to update file
    }
    update.close();
}

// copy master file
void createCopyFile(char* fileName) {
    string nameMaster = fileName;
    system(("cp "+nameMaster+" copy.out").c_str()); // create copy of file in system 
}

// delete copy file
void deleteCopyFile() {
    system(("rm copy.out"));
}


// prints entire file/list of records
void printFile(char* fileName) {
    // print records from binary file
    // credit to Dr. Digh for the for loops and printRecord()
    fstream binaryFile(fileName, ios::in|ios::binary); 
    for (int i=0; i<80; i++) cout<<'^'; cout<<endl;    
    BookRec* record = new BookRec;
    while(binaryFile.read((char*) record, sizeof(BookRec))) {
        printRecord(*record);
    }
    for (int i=0; i<80; i++) cout<<'^'; cout<<endl;
    binaryFile.close();
}

// prints single record
void printRecord(BookRec record) {
    // credit to Dr. Digh for this print function
    cout<<setw(10)<<setfill('0')<<record.isbn
	    <<setw(25)<<setfill(' ')<<record.name
	    <<setw(25)<<record.author
	    <<setw(3)<<record.onhand
	    <<setw(6)<<record.price
	    <<setw(10)<<record.type<<endl;
}

// add additional zeros and other characters for printing purposes
string keyFormat(unsigned int& key) {
    // convert to string
    string keyString = toString(key);
    while(keyString.length()<10)
        keyString = "0"+keyString;
    return keyString;
}

// to string function
string toString(unsigned int& key) {
    stringstream ss;
    ss<<key;
    return ss.str();
}
