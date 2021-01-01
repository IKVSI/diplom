#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
using namespace std;
class ReadFile
{
private:
	string filename;
	ifstream fin;
	unsigned long long filesize;
	const unsigned long long buffersize = 20;
	char * buffer;
	unsigned long long curentBlock = 1;
	unsigned long long maxblock = 0;
	void readBlcok(unsigned long long block);
public:
	ReadFile(string filename);
	~ReadFile();
	string getStrBuffer();
	char * readBytes(unsigned long long start, unsigned long long length);
};
