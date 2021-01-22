#include "ReadFile.h"
// Конструктор
ReadFile::ReadFile(string filename)
{
	this->filename = filename;
	this->fin = ifstream(this->filename, ios::in | ios::binary);
	this->fin.seekg(0, ios_base::end);
	this->filesize = this->fin.tellg();
	this->maxblock = this->filesize / this->buffersize;
	this->readBlock(0);
}

// Деструктор
ReadFile::~ReadFile()
{
	for(auto i = this->buffers.begin(); i != this->buffers.end(); ++i)
    {
	    delete [] i->second;
    }
}

void ReadFile::readBlock(unsigned long long int block)
{
    if (block != this->curentBlock)
    {
        if (block > this->maxblock) block = this->maxblock;
        auto i = this->buffers.find(block);
        if (i == this->buffers.end())
        {
            this->fin = ifstream(this->filename, ios::in | ios::binary);
            this->fin.seekg(block * this->buffersize, ios::beg);
            this->buffer = new char[this->buffersize];
            this->fin.read(this->buffer, this->buffersize);
            this->buffers.emplace(block, this->buffer);
        }
        else this->buffer = this->buffers[block];
        this->curentBlock = block;
    }
}

string ReadFile::strBytes(char *bts, unsigned long long size)
{
    stringstream temp;
    temp << "[ ";
    if (size) {
        unsigned long long tbs = size - 1;
        for (unsigned long long i = 0; i < tbs; ++i)
        {
            temp << setw(2) << setfill('0') << hex << (unsigned int) (unsigned char) (bts[i]) << ", ";
        }
        temp << setw(2) << setfill('0') << hex << (unsigned int) (unsigned char) (bts[tbs]);
    }
    temp << " ]";
    return temp.str();
}

unsigned char * ReadFile::readBytes(unsigned long long int start, unsigned long long int &length)
{
    if (start > this->filesize)
    {
        length = 0;
        return nullptr;
    }
    unsigned long long int end = start + length - 1;
    if ( (start > end) || (end > this->filesize))
    {
        length = this->filesize - start;
        end = this->filesize - 1;
    }
    char * rbts = new char [length];
    char * cursor = rbts;
    while (start <= end)
    {
        unsigned long long int blockstart = start / this->buffersize;
        this->readBlock(blockstart);
        unsigned long long int blockend = end / this->buffersize;
        unsigned long long int st = start % this->buffersize;
        unsigned long long int ed = (blockstart == blockend) ? (end % this->buffersize) : this->buffersize - 1;
        unsigned long long int cpsize = ed - st + 1;
        memcpy(cursor, &this->buffer[st], cpsize);
        start += cpsize;
        cursor += cpsize;
    }
    return (unsigned char *) rbts;
}

unsigned long long int ReadFile::getFileSize()
{
    return this->filesize;
}

string ReadFile::getFileName()
{
    return this->filename;
}

