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

string bin(unsigned long long a, unsigned char bits)
{
    stringstream temp;
    for(unsigned long i = (1 << (bits - 1)); i != 0; i >>= 1)
    {
        short p = (a&i) ? 1 : 0;
        temp << p;
    }
    return temp.str();
}

string binn(unsigned long long a)
{
    stringstream temp;
    for(unsigned long long i = 0x8000000000000000; i != 0; i >>= 1)
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

bool Huffman::decodeCategory(unsigned char category, unsigned long long int &buffer, unsigned short &btlength, signed long &decodenum)
{
    if (category > btlength) return false;
    btlength -= category;
    unsigned long long temp = buffer >> btlength;
    buffer &= ((1 << btlength) - 1);
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
    vector<unsigned short> bts;
};

void Huffman::createFromFrequencies(map<unsigned short, unsigned long long int> freq)
{
    freq.emplace(0x100, 0);
    vector<Items> K;
    unsigned long long b = 0;
    for(auto key = freq.begin(); key != freq.end(); ++key)
    {
        b += key->second;
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
    while(x <= X)
    {
        ++d0;
        x <<= 1;
    }
    --d0;
    x >>= 1;
    map<signed char, vector <unsigned long>> L;
    for(unsigned long tempx = X; d0 != - this->bitlength - 1; --d0, x >>= 1)
    {
        if ((tempx) && (tempx >= x))
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
                    if (K[(*key)].weight < t.weight) break;
                    else if ((K[(*key)].weight == t.weight) && (K[(*key)].bts.size() < t.bts.size())) break;
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
        if (key->first != 0x100) P.emplace(key->first, 0);
    }
    for(auto key = S.begin(); key != S.end(); ++key)
    {
        for(auto jey = K[*key].bts.begin(); jey != K[*key].bts.end(); ++jey)
        {
            if (*jey != 0x100) ++P[(*jey)];
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
            this->codes.emplace(this->symbols[j], counter);
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

string Huffman::showData()
{
    unsigned short hlength = this->bitlength;
    stringstream  temp;
    --hlength;
    unsigned short k = 0;
    temp << "[ ";
    for(unsigned short i = 0; i < hlength; ++i)
    {
        temp << i + 1 << ": ( ";
        if (counts[i])
        {
            unsigned char cnt = counts[i] - 1;
            for(unsigned short j = 0; j < cnt; ++j)
            {
                temp << (unsigned short) symbols[k] << ", ";
                ++k;
            }
            temp << (unsigned short) symbols[k];
            ++k;
        }
        temp << " ), ";
    }
    temp << hlength + 1 << ": ( ";
    if (counts[hlength])
    {
        unsigned char cnt = counts[hlength] - 1;
        for(unsigned short j = 0; j < cnt; ++j)
        {
            temp << (unsigned short) symbols[k] << ", ";
            ++k;
        }
        temp << (unsigned short) symbols[k];
        ++k;
    }
    temp << " ) ";
    temp << "]";
    return temp.str();
}

unsigned char *Huffman::huffmanSave(unsigned long long & size)
{
    size = this->counts.size() + this->symbols.size();
    unsigned char * r = new unsigned char[size];
    unsigned long long j = 0;
    for(auto key : this->counts) r[j++] = key;
    for(auto key : this->symbols) r[j++] = key;
    return r;
}

 void Huffman::encode(unsigned long long int &a, unsigned short &btlength, unsigned char &decodesym)
{
    //cout << "ENC\n";
    //cout << "BIN: " << binn(a) << " LENGTH: " << btlength << '\n';
    unsigned long long b = this->codes[decodesym];
    unsigned char bits = this->codelength[decodesym];
    a <<= bits;
    a |= b;
    btlength += bits;
    //cout << "BIN: " << binn(a) << " LENGTH: " << btlength << '\n';
}

void Huffman::encodeCategory(unsigned char category, unsigned long long int &buffer, unsigned short &bitlength, long int &encodenum)
{
    //cout << "ENCCAT\n";
    //cout << "BIN: " << binn(buffer) << " LENGTH: " << bitlength << '\n';
    buffer <<= category;
    bitlength += category;
    if (encodenum > 0) buffer |=encodenum;
    else buffer |= (encodenum + ((1 << category) - 1));
    //cout << "BIN: " << binn(buffer) << " LENGTH: " << bitlength << '\n';
}

