#include "ReadFile.h"

void ReadFile::readBlcok(unsigned long long block)
{
	if (block != this->curentBlock)
	{
		if (block > this->maxblock) block = this->maxblock;
		this->fin.seekg(block * this->buffersize);
		this->fin.read(this->buffer, this->buffersize);
		this->curentBlock = block;
	}
}

ReadFile::ReadFile(string filename)
{
	this->filename = filename;
	this->fin = ifstream(this->filename);
	this->fin.seekg(0, ios_base::end);
	this->filesize = this->fin.tellg();
	this->maxblock = this->filesize / this->buffersize;
	this->buffer = new char[this->buffersize];
	this->readBlcok(0);
}



ReadFile::~ReadFile()
{
	delete[] this->buffer;
}

string ReadFile::getStrBuffer()
{
	stringstream temp;
	temp << "[ ";
	unsigned long long tbs = this->buffersize - 1;
	for (unsigned long long i = 0; i < tbs; ++i)
	{
		temp << setw(2) << setfill('0') << hex << (unsigned int)(unsigned char)(this->buffer[i]) << ", ";
	}
	temp << setw(2) << setfill('0') << hex << (unsigned int)(this->buffer[tbs]) << " ]\n";
	return temp.str();
}

char * readBytes(unsigned long long start, unsigned long long length)
{
	
	return nullptr;
}
