//
// Created by kvs on 13.01.2021.
//

#include "Huffman.h"

Huffman::Huffman(unsigned short bitlength)
{
    this->bitlength = bitlength;
    this->size = 1 << this->bitlength;
    this->table = new unsigned char[this->size];
}

Huffman::~Huffman()
{
    if (this->table != nullptr) delete [] this->table;
}

void Huffman::createFromJPEG(unsigned char *counts, unsigned char *symbols, unsigned short size)
{
    unsigned long next = 0;
    unsigned long last = 0;
    for(unsigned short i = 0, j = 0; i < this->bitlength;)
    {
        if (j != counts[i])
        {
            cout << next << "\n";
            unsigned long nxt = next << (bitlength - 1 - i);
            this->table[nxt] = symbols[j];
            this->codelength.emplace(symbols[j], i+1);
            last = nxt;
            for(unsigned long k = last + 1; k < nxt; ++k)
            {
                this->table[k] = this->table[last];
            }
            ++next;
            ++j;
        }
        else
        {
            next << 1;
            j = 0;
            ++i;
        }
    }
    for(unsigned long k = last + 1; k < this->size; ++k)
    {
        this->table[k] = this->table[last];
    }
}

string bin(unsigned long a)
{
    stringstream temp;
    for(unsigned long i = 0x80000000; i != 0; i >>= 1)
    {
        short p = (a&i) ? 1 : 0;
        temp << p;
    }
    return temp.str();
}

string Huffman::showTable()
{
    stringstream temp;
    for(unsigned long i = 0; i < this->size; ++i)
    {
        temp << bin(i) << ": " << hex << (unsigned short) this->table[i] << " length: " << this->codelength[this->table[i]] << "\n";
    }
    return temp.str();
}
