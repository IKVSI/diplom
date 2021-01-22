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

template <typename T, typename  V>
string strmap(map<T, V> mp)
{
    stringstream temp;
    temp << "{ ";
    for(auto i = mp.begin(); i != mp.end(); ++i)
    {
        temp << "0x"<<hex << (unsigned short) i->first << ": " << dec<< (unsigned short) i->second << ", ";
    }
    temp << '\b' << '\b' << '}';
    return temp.str();
}

void Huffman::createFromJPEG(unsigned char *counts, unsigned char *symbols)
{
    /*
    unsigned long next = 0;
    unsigned long last = 0;
    unsigned short i = 0, j = 0;
    cout << dec << "[ ";
    while(i < this->bitlength - 1)
    {
        cout << i+1 << " : " <<(unsigned short) counts[i] << "/(";
        if (counts[i])
        {
            unsigned short k = 0;
            while (k < counts[i] - 1)
            {
                cout << (unsigned short)symbols[j] << ", ";
                ++j;
                ++k;
            }
            cout << (unsigned short)symbols[j];
            ++j;
        }
        cout << "), ";
        ++i;
    }
    cout << i+1 << " : " << (unsigned short)counts[i] << "(";
    if (counts[i])
    {
        unsigned short k = 0;
        while (k < counts[i] - 1)
        {
            cout << (unsigned short)symbols[j] << ", ";
            ++j;
            ++k;
        }
        cout << (unsigned short)symbols[j];
        ++j;
    }
    cout <<  ")";
    cout << "]\n";
     */
    unsigned long long counter = 0;
    for(unsigned char i = 0, j = 0, k = 0; i < this->bitlength;)
    {
        //cout << counter<<" "<<hex<<(unsigned short) symbols[j]<< " "<<dec<<(unsigned short) k<<"/" <<(unsigned short) counts[i] <<" " <<(unsigned short) i<<   '\n';
        if (k != counts[i])
        {
            unsigned long long ts = counter << (this->bitlength - i - 1);
            ++counter;
            unsigned long long te = counter << (this->bitlength - i - 1);
            for(unsigned long long ti = ts; ti < te; ++ti) this->table[ti] = symbols[j];
            this->codelength.emplace(symbols[j], i + 1);
            ++j;
            ++k;
        }
        else
        {
            counter <<= 1;
            ++i;
            k = 0;
        }
    }
}

string bin(unsigned long a, unsigned char bits)
{
    stringstream temp;
    for(unsigned long i = (1 << (bits - 1)); i != 0; i >>= 1)
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
        temp << bin(i, this->bitlength) << ": " << hex << (unsigned short) this->table[i] << " length: " << dec <<(unsigned short) this->codelength[this->table[i]] << "\n";
    }
    return temp.str();
}

bool Huffman::decode(unsigned long long &a, unsigned short &bitlength, unsigned char &decodesym)
{
    unsigned long long b = a;
    if (bitlength > this->bitlength) b >>= (bitlength - this->bitlength);
    else b <<= (this->bitlength - bitlength);
    decodesym = this->table[b];
    unsigned char length = this->codelength[decodesym];
    unsigned short btl = bitlength - length;
    if (btl < 64)
    {
        bitlength = btl;
        b = 1;
        b <<= bitlength;
        b -= 1;
        a &= b;
        return true;
    }
    return false;
}

bool Huffman::decodeCategory(unsigned char category, unsigned long long int &buffer, unsigned short &bitlength, signed long long &decodenum)
{
    if (category > bitlength) return false;
    bitlength -= category;
    unsigned long long temp = buffer >> bitlength;
    buffer &= ((1 << bitlength) - 1);
    if (category) {
        decodenum = (1 << (category - 1));
        if (temp >= decodenum) decodenum = temp;
        else
        {
            decodenum <<= 1;
            decodenum = -decodenum + 1 + temp;
        }
    }
    else decodenum = 0;
    return true;
}
