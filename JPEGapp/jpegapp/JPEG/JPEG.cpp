#include "JPEG.h"

union twobyte
{
    unsigned short int * num;
    unsigned char * bts;
};

JPEG::JPEG(string filename)
{
    this->fin = new ReadFile(filename);
    JPEG::MARKERS[0] = &JPEG::ignoreMarker;
    std::fill(JPEG::MARKERS+1, JPEG::MARKERS+256, &JPEG::defaultMarker);
    std::fill(JPEG::MARKERS+0xd0, JPEG::MARKERS+0xd8, &JPEG::ignoreMarker);
    JPEG::MARKERS[0xd8] = &JPEG::SOIMarker;
    JPEG::MARKERS[0xd9] = &JPEG::EOIMarker;
    JPEG::MARKERS[0xc4] = &JPEG::DHTMarker;
}

JPEG::~JPEG()
{
    delete this->fin;
    for(auto key = this->DC.begin(); key != this->DC.end(); ++key)
    {
        delete key->second;
    }
    for(auto key = this->AC.begin(); key != this->AC.end(); ++key)
    {
        delete key->second;
    }
}

void JPEG::findMarkers()
{
    cout << "\t\tLook for markers! :)\n\n";
    this->cursor = 0;
    if (this->isMarker())
    {
        ++this->cursor;
        this->runMarker();
    }
    if (this->startimage) while(this->cursor < this->fin->getFileSize())
    {
        if (this->isMarker())
        {
            ++this->cursor;
            this->runMarker();
        }
        else ++this->cursor;
        if (this->endimage) break;
    }
    else
    {
        cerr << "Marker Start of Image not found!\n";
        raise(1);
    }
}

void JPEG::ignoreMarker()
{
    ++this->cursor;
}

unsigned long long int JPEG::markerLength()
{
    twobyte length;
    this->readlength = 2;
    length.bts = this->fin->readBytes(this->cursor, this->readlength, true);
    unsigned long long int temp = *length.num;
    delete [] length.bts;
    return temp;
}

void JPEG::pushMarker(unsigned char marker, unsigned long long int addr)
{
    this->markerstype.push_back(marker);
    this->markersaddr.push_back(addr);
}

bool JPEG::isMarker()
{
    this->readlength = 1;
    unsigned char * ff = this->fin->readBytes(this->cursor, this->readlength);
    bool ok = (ff[0] == 0xFF);
    delete [] ff;
    return ok;
}

void JPEG::defaultMarker()
{
    this->readlength = 1;
    unsigned char * marker = this->fin->readBytes(this->cursor, this->readlength);
    cout << "Found Marker " << ReadFile::strBytes((char*)marker, this->readlength);
    cout << " at 0x" << setw(8) << setfill('0') << hex << this->cursor << " !\n";
    this->pushMarker(marker[0], this->cursor - 1);
    delete [] marker;
    ++this->cursor;

    unsigned long long int length = this->markerLength();
    this->cursor += length;
    if ((this->cursor < this->fin->getFileSize()) && (this->isMarker()))
    {
        cout << "\tLength: " << dec << length << '\n';
    }
    else this->cursor -= length;
}

void JPEG::runMarker()
{
    unsigned char * marker = this->fin->readBytes(this->cursor, this->readlength);
    if (this->readlength)
    {
        jpegfunc func = JPEG::MARKERS[marker[0]];
        (this->*func)();
        delete[] marker;
    }
    else
    {
        cerr << "Broken File !\n";
        raise(2);
    }
}

void JPEG::SOIMarker()
{
    cout << "Found Start of Image Marker [ d8 ] at 0x" << setw(8) << setfill('0') << hex << this->cursor << " !\n";
    this->markerstype.push_back(0xd8);
    this->markersaddr.push_back(this->cursor - 1);
    ++this->cursor;
    this->startimage = true;
    cout << '\n';
}

void JPEG::EOIMarker()
{
    cout << "Found End of Image Marker [ d9 ] at 0x" << setw(8) << setfill('0') << hex << this->cursor << " !\n" <<flush;
    this->markerstype.push_back(0xD9);
    this->markersaddr.push_back(this->cursor - 1);
    ++this->cursor;
    this->endimage = true;
    cout << '\n';
}

bool check(unsigned char a)
{
    const unsigned char cmsize = 6;
    const unsigned char clearmarkers[cmsize] = {0xC0, 0xC4, 0xD8, 0xDA,  0xDB, 0xDD};
    return binary_search(clearmarkers, clearmarkers+cmsize, a);
}

void JPEG::saveClearJpeg()
{
    this->findMarkers();
    string foutfilename = this->fin->getFileName();
    foutfilename = foutfilename.substr(0, foutfilename.size() - 4) + ".clear.jpg";
    ofstream fout(foutfilename, ios::binary|ios::out);
    this->cursor = 0;
    bool ok = true;
    for(unsigned int i = 0, size = this->markerstype.size() - 1; i < size; ++i)
    {
        if (check(this->markerstype[i]))
        {
            unsigned long long int start = this->markersaddr[i];
            unsigned long long int length = this->markersaddr[i + 1] - start;
            unsigned char * bts = this->fin->readBytes(start, length);
            fout.write((char *)bts, length);
            delete [] bts;
        }
    }
    char end[] = {'\xFF', '\xD9'};
    fout.write(end, 2);
    cout << "FILE SAVED TO \"" << foutfilename <<"\"\n";
}

void JPEG::DHTMarker()
{
    cout << "Find Define Huffman Table marker [ c4 ] at 0x" << setw(8) << setfill('0') << hex << this->cursor << " !\n";
    ++this->cursor;
    unsigned long long int length = this->markerLength();
    unsigned long long int endcursor = this->cursor += length;
    this->cursor += 2;
    while (this->cursor != endcursor)
    {
        this->readlength = 1;
        unsigned char * tp = this->fin->readBytes(this->cursor, this->readlength);
        unsigned short number = tp[0] & 0x0F;
        bool ac = (tp[0] & 0xF0);
        delete [] tp;

        unsigned char counts[hufsize];
        unsigned short sum = 0;
        for(unsigned short i = 0; i < hufsize; ++i)
        {
            unsigned char * num = this->fin->readBytes(this->cursor, this->readlength);
            counts[i] = num[0];
            sum += counts[i];
            delete [] num;
        }

        unsigned char * symbols = new unsigned char[sum];
        for(unsigned short i = 0; i < sum; ++i)
        {
            unsigned char *sym = this->fin->readBytes(this->cursor, this->readlength);
            symbols[i] = sym[0];
            delete[] sym;
        }
        Huffman * H = new Huffman(hufsize);
        H->createFromJPEG(counts, symbols, sum);
        //cout << H->showTable();
        if (ac)
        {
            this->AC.emplace(number, H);
        }
        else
        {
            this->DC.emplace(number, H);
        }
        delete [] symbols;
    }
}

