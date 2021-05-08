#include<cstdlib>
#include<iostream>
#include<fstream>
#include<sstream>
#include<climits>
#include<iomanip>
#include<map>
#include<sstream>
#include<cstring>

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

void printRecord(BookRec* record) {
    // credit to Dr. Digh for this print function
    cout<<setw(10)<<setfill('0')<<record->isbn
	    <<setw(25)<<setfill(' ')<<record->name
	    <<setw(25)<<record->author
	    <<setw(3)<<record->onhand
	    <<setw(6)<<record->price
	    <<setw(10)<<record->type<<endl;
}

void printFile(char* fileName) {
    // print records from binary file
    // credit to Dr. Digh for the for loops and printRecord()
    fstream binaryFile(fileName, ios::in|ios::binary); 
    for (int i=0; i<80; i++) cout<<'^'; cout<<endl;    
    TransactionRec* record = new TransactionRec;
    while(binaryFile.read((char*) record, sizeof(TransactionRec))) {
        printRecord(&(record->B));
    }
    for (int i=0; i<80; i++) cout<<'^'; cout<<endl;
    binaryFile.close();
}



int main(int argc, char** argv) {
    printFile(argv[1]);
    return 0;
}