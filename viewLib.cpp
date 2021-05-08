#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>

using namespace std;

typedef	char String[25];

struct BookRec
{
  unsigned int isbn;
  String name;
  String author;
  int onhand;
  float price;
  String type;
};

int main(int argc, char* argv[])
{

  cout.setf(ios::fixed, ios::floatfield);
  cout.setf(ios::showpoint);
  cout.precision(2);

  system("clear");

  string fileName;

  if (argc==1)
  {
	cout<<"What file do you wish to view?"<<endl;
	cout<<endl;
	system("ls");
	cout<<endl;
	cout<<"?> ";
	cin>>fileName;
  }
  else
  {
	fileName = argv[1];
  }

  fstream in(fileName.c_str(), ios::in | ios::binary);

  if (in.fail())
  {
	cout<<endl;
	cout<<"### Cannot open file "<<fileName<<endl;
	cout<<endl;
  }
  else
  {

	in.seekg(0, ios::beg);

	BookRec	buf;

	system("clear");
	cout<<"Content of "<<fileName<<":"<<endl;
	for (int i=0; i<80; i++) cout<<'^'; cout<<endl;

	while (in.read((char *) &buf, sizeof(BookRec) ) )
	{
	  cout<<setw(10)<<setfill('0')<<buf.isbn
	      <<setw(25)<<setfill(' ')<<buf.name
	      <<setw(25)<<buf.author
	      <<setw(3)<<buf.onhand
	      <<setw(6)<<buf.price
	      <<setw(10)<<buf.type<<endl;
	}

	for (int i=0; i<80; i++) cout<<'^'; cout<<endl;
  } 
  cout<<endl;
  return 0;
}
