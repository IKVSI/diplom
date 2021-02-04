
#ifndef JPEGAPP_HUFFMAN_H
#define JPEGAPP_HUFFMAN_H

#include "ReadFile.h"
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <iostream>
using namespace std;

class Huffman
{
private:
    unsigned short bitlength;
    unsigned long size;
    unsigned char * table;
    vector <unsigned char> counts;
    vector <unsigned char> symbols;
    void createFromClass();
public:
    bool static decodeCategory(unsigned char category, unsigned long long int &buffer, unsigned short &bitlength, signed long long &decodenum);
    Huffman(unsigned short bitlength);
    bool decode(unsigned long long &a, unsigned short &btlength, unsigned char &decodesym);
    map<unsigned char, unsigned char> codelength;
    string showTable();
    void createFromJPEG(unsigned char * counts, unsigned char * symbols);
    void createFromFrequencies(map<unsigned char, unsigned long long> freq);
    string showData();
    ~Huffman();
};


#endif //JPEGAPP_HUFFMAN_H
