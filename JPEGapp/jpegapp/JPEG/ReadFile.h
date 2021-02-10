#ifndef JPEGAPP_READFILE_H
#define JPEGAPP_READFILE_H
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <climits>
#include <cstring>
#include <map>

using namespace std;
class ReadFile
{
private:
	string filename;
	ifstream fin;
	unsigned long long filesize;
	const unsigned long long buffersize = 2048;
	char * buffer;
	map<unsigned long long, char *> buffers;
	unsigned long long curentBlock = 1;
	unsigned long long maxblock = 0;
	void readBlock(unsigned long long block);
public:
	ReadFile(string filename);
	~ReadFile();
	static string strBytes(char * bts, unsigned long long size);
	unsigned char * readBytes(unsigned long long start, unsigned long long &length);
	unsigned long long int getFileSize();
	string getFileName();
};

#endif //JPEGAPP_READFILE_H