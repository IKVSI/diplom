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
    this->findMarkers();
}

JPEG::~JPEG()
{
    delete this->fin;
}

void JPEG::defaultMarker()
{
    this->readlength = 1;
    unsigned char * marker = this->fin->readBytes(this->cursor, this->readlength);
    cout << "Found Marker " << ReadFile::strBytes((char*)marker, this->readlength);
    cout << " at 0x" << setw(8) << setfill('0') << hex << this->cursor << " !\n";
    this->markerstype.push_back(marker[0]);
    this->markersaddr.push_back(this->cursor - 1);
    delete [] marker;
    ++this->cursor;
    this->readlength = 2;
    twobyte length;
    length.bts = this->fin->readBytes(this->cursor, this->readlength, true);
    if (this->readlength)
    {
        unsigned long long int lastcursor = this->cursor;
        this->cursor += *length.num;
        if ((this->cursor < this->fin->getFileSize()) && (this->isMarker()))
        {
            cout << "\tLength: " << dec <<*length.num << " bytes: " << ReadFile::strBytes((char *) length.bts, 2) << '\n';
        }
        else
        {
            this->cursor = lastcursor;
        }
    }
    delete [] length.bts;
    cout << '\n';
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

bool JPEG::isMarker()
{
    this->readlength = 1;
    unsigned char * ff = this->fin->readBytes(this->cursor, this->readlength);
    bool ok = (ff[0] == 0xFF);
    delete [] ff;
    return ok;
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

