
#ifndef JPEGAPP_HUFFMAN_H
#define JPEGAPP_HUFFMAN_H

#include <string>
#include <sstream>
#include <map>
#include <iomanip>
#include <iostream>
using namespace std;

class Huffman
{
private:
    unsigned short bitlength;
    unsigned long size;
    unsigned char * table;
public:
    Huffman(unsigned short bitlength);
    map<char, char> codelength;
    string showTable();
    void createFromJPEG(unsigned char * counts, unsigned char * symbols, unsigned short size);
    ~Huffman();
};


#endif //JPEGAPP_HUFFMAN_H
