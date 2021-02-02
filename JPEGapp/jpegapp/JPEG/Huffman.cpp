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
    for(unsigned char i = 0, j = 0; i < this->bitlength; ++i)
    {
        this->counts.push_back(counts[i]);
        if (counts[i])
        {
            for(unsigned char k = 0; k < counts[i]; ++k)
            {
                this->symbols.push_back(symbols[j]);
                ++j;
            }
        }
    }
    this->createFromClass();
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

struct Items
{
    unsigned long long weight;
    vector<unsigned char> bts;
};

void Huffman::createFromFrequencies(map<unsigned char, unsigned long long int> freq)
{
    vector<Items> K;
    for(auto key = freq.begin(); key != freq.end(); ++key)
    {
        Items a;
        a.weight = key->second;
        a.bts.push_back(key->first);
        K.push_back(a);
    }
    sort(K.begin(), K.end(), [](Items a, Items b) { return a.weight > b.weight; });
    unsigned long X = K.size() - 1;
    vector<unsigned long> S;
    vector<signed char> D;
    vector<bool> M;
    signed char d0 = 0;
    unsigned long x = 1;
    while(x < X)
    {
        ++d0;
        x <<= 1;
    }
    --d0;
    x >>= 1;
    map<signed char, vector <unsigned long>> L;
    for(unsigned long tempx = X; d0 != - this->bitlength - 1; --d0, x >>= 1)
    {
        if ((x) && (tempx >= x))
        {
            tempx -= x;
            M.push_back(1);
        }
        else M.push_back(0);
        D.push_back(d0);
        if (d0 < 0) for(unsigned long i = 0; i < K.size(); ++i) L[d0].push_back(i);
    }
    while (M[0])
    {
        signed char d = D.back();
        bool m = M.back();
        if (m)
        {
            S.push_back(L[d].back());
            L[d].pop_back();
            M.pop_back();
            M.push_back(false);
        }
        else
        {
            while (L[d].size() > 1)
            {
                unsigned long a = L[d].back();
                L[d].pop_back();
                unsigned long b = L[d].back();
                L[d].pop_back();
                Items t;
                t.weight = K[a].weight + K[b].weight;
                t.bts.insert(t.bts.end(), K[a].bts.begin(), K[a].bts.end());
                t.bts.insert(t.bts.end(), K[b].bts.begin(), K[b].bts.end());
                auto key = L[d+1].begin();
                while (key != L[d+1].end())
                {
                    if (K[(*key)].weight <= t.weight) break;
                    ++key;
                }
                L[d+1].insert(key, K.size());
                K.push_back(t);
            }
            D.pop_back();
            M.pop_back();
        }
    }
    map<unsigned char, unsigned char> P;
    for(auto key = freq.begin(); key != freq.end(); ++key)
    {
        P.emplace(key->first, 0);
    }
    for(auto key = S.begin(); key != S.end(); ++key)
    {
        for(auto jey = K[*key].bts.begin(); jey != K[*key].bts.end(); ++jey)
        {

            ++P[(*jey)];
        }
    }
    for(unsigned char i = 0; i < this->bitlength; ++i) this->counts.push_back(0);
    unsigned long sum = 0;
    for(auto key = P.begin(); key != P.end(); ++key)
    {
        ++this->counts[key->second - 1];
        ++sum;
    }
    for(unsigned char i = 0; i < this->bitlength; ++i)
    {
        if (this->counts[i])
        {
            for(auto key = P.begin(); key != P.end(); ++key)
            {
                if (key->second == (i + 1)) this->symbols.push_back(key->first);
            }
        }
    }
    this->createFromClass();
}

void Huffman::createFromClass()
{
    unsigned long long counter = 0;
    for(unsigned char i = 0, j = 0, k = 0; i < this->bitlength;)
    {
        if (k != this->counts[i])
        {
            unsigned long long ts = counter << (this->bitlength - i - 1);
            ++counter;
            unsigned long long te = counter << (this->bitlength - i - 1);
            for(unsigned long long ti = ts; ti < te; ++ti)
            {
                this->table[ti] = this->symbols[j];
            }
            this->codelength.emplace(this->symbols[j], i + 1);
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
