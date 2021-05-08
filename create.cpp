#include<cstdlib>
#include<iostream>
#include<fstream>
#include<sstream>
#include<climits>
#include<iomanip>

using namespace std;

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

struct BookRec;

bool checkArgs(int& argc, char** argv); // check if input file can be opened
void outputRecords(ifstream& inFile, fstream& binaryFile); // grabs next record from input .txt file
void printFile(const char* fileName); // inputs data from given binary file and outputs to cout
void printRecord(BookRec* record); // prints record

int main(int argc, char** argv) {
    system("clear");

    // files
    string oFileString = "library.out"; const char* oFile = oFileString.c_str();

    if(!checkArgs(argc, argv)) return 1; // failure: either file not found or invalid num arguments
    ifstream inFile(argv[1]); // input file
    fstream binaryFile(oFile, ios::out|ios::binary); // in/out/binary mode
    outputRecords(inFile, binaryFile);
    printFile(oFile);
    return 0;
}

bool checkArgs(int& argc, char** argv) {
    if(argc>2) {
	cerr<<"\tERR: Invalid arguments in command."<<endl;
	cout<<"\tSyntax: ./a.out [input file]"<<endl;
    	return false;
    }
    ifstream inFile(argv[1]);
    if(!inFile.is_open()) { // file not found
	inFile.close();
	cerr<<"\tERR: Invalid file name. File '"<<(argv[1]==NULL?" ":argv[1])<<"' not found."<<endl;
	return false;
    }
    inFile.close(); // close file
    return true;
}

void outputRecords(ifstream& inFile, fstream& binaryFile) {
    BookRec* record = new BookRec; record->isbn = 0;

    // gather in put from file into record
    long isbn; long prevISBN = INT_MIN;
    int lineCount = 1;
    while(inFile>>isbn) {
        // input
        bool output = true;
        record->isbn = isbn;
        inFile.ignore(1, '|'); // ignore pipe char in input
        inFile.getline(record->name, 25, '|'); // author
        inFile.getline(record->author, 25, '|'); // name
        inFile>>record->onhand; inFile.ignore(1, '|'); // onhand
        inFile>>record->price; inFile.ignore(1, '|'); // price
        inFile.getline(record->type, 25); // genre

        if(isbn<1) {
            cerr<<"Illegal ISBN detected on line "<<lineCount<<" of data file - record ignored."<<endl;
            output = false;
        }
        else if(isbn<=prevISBN) {
            cerr<<"ISBN out of sequence on line "<<lineCount<<" of data file."<<endl;
            printRecord(record);
        }

        if(record->onhand<0) {
            cerr<<"Negative amount onhand detected on line "<<lineCount<<" - record ignored."<<endl;
            printRecord(record);
            output = false;
        }
        
        if(record->price<0) {
            cerr<<"Negative price detected on line "<<lineCount<<" - record ignored."<<endl;
            printRecord(record);
            output = false;
        }

        // write out to binary file
        if(output) {
            prevISBN = isbn;
            binaryFile.write((char*) record, sizeof(BookRec));
        }
        lineCount++;
    }
    binaryFile.close();
}

void printFile(const char* fileName) {
    // print records from binary file
    // credit to Dr. Digh for the for loops and printRecord()
    fstream binaryFile(fileName, ios::in|ios::binary); 
    for (int i=0; i<80; i++) cout<<'^'; cout<<endl;    
    BookRec* record = new BookRec;
    while(binaryFile.read((char*) record, sizeof(BookRec))) {
        printRecord(record);
    }
    for (int i=0; i<80; i++) cout<<'^'; cout<<endl;
    binaryFile.close();
}

void printRecord(BookRec* record) {
    // credit to Dr. Digh for this print function
    cout<<setw(10)<<setfill('0')<<record->isbn
	    <<setw(25)<<setfill(' ')<<record->name
	    <<setw(25)<<record->author
	    <<setw(3)<<record->onhand
	    <<setw(6)<<record->price
	    <<setw(10)<<record->type<<endl;
}
