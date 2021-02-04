#include "JPEG.h"

JPEG::JPEG(string filename)
{
    this->fin = new ReadFile(filename);
    fill(JPEG::MARKERS, JPEG::MARKERS+256, &JPEG::defaultMarker);
    JPEG::MARKERS[0xd8] = &JPEG::markerSOI;
    JPEG::MARKERS[0xd9] = &JPEG::markerEOI;
    JPEG::MARKERS[0xdd] = &JPEG::markerDRI;
    JPEG::MARKERS[0xc0] = &JPEG::markerSOF;
    JPEG::MARKERS[0xc4] = &JPEG::markerDHT;
    JPEG::MARKERS[0xda] = &JPEG::markerSOS;
    JPEG::MARKERS[0xdb] = &JPEG::markerDQT;
}

unsigned long long JPEG::readNumFromFile(unsigned char length)
{
    unsigned long long l = length;
    unsigned char * bts = this->fin->readBytes(this->cursor, l);
    if (l != length)
    {
        if (l) delete [] bts;
        cerr << "Broken file: Failed read " << (unsigned short) length << "b" << '\n' << flush;
        raise(4);
    }
    l = 0;
    for(unsigned char i = 0; i < length; ++i)
    {
        l <<= 8;
        l |= bts[i];
    }
    delete [] bts;
    this->cursor += length;
    return l;
}

JPEG::~JPEG()
{
    delete this->fin;
    for(auto key = this->components.begin(); key != this->components.end(); ++key) delete *key;
    for(auto key = this->AC.begin(); key != this->AC.end(); ++key) delete key->second;
    for(auto key = this->DC.begin(); key != this->DC.end(); ++key) delete key->second;
    for(auto key = this->QT.begin(); key != this->QT.end(); ++key)
    {
        for(unsigned char i = 0; i < this->qtprec[key->first]; ++i) delete [] key->second[i];
        delete [] key->second;
    }
}

void JPEG::findMarkers()
{
    this->cursor = 0;
    while (this->cursor < this->fin->getFileSize())
    {
        unsigned char marker = this->readNumFromFile(1);
        if (marker == 0xFF)
        {
            marker = this->readNumFromFile(1);
            if (!binary_search(this->neMARKER, this->neMARKER + 9, marker))
            {
                this->markerstype.push_back(marker);
                this->markersaddr.push_back(this->cursor - 2);
                jpegfunc func = JPEG::MARKERS[marker];
                (this->*func)();
            }
        }
    }
}

string strmarker(unsigned short marker, unsigned long long cursor)
{
    stringstream temp;
    temp << "\"0x" << hex << setw(2) << setfill('0') << marker;
    temp << "\" at 0x" << setw(8) << cursor;
    return temp.str();
}

unsigned short JPEG::findLength()
{
    unsigned short length = this->readNumFromFile(2) - 2;
    unsigned long long cursor = this->cursor;
    this->cursor += length;
    unsigned char marker = this->readNumFromFile(1);
    if (marker == 0xFF)
    {
        cout << "\tLength: " << length<<'\n';
        this->cursor = cursor;
    }
    else
    {
        length = 0;
        this->cursor = cursor - 2;
    }
    return length;
}

void JPEG::defaultMarker()
{
    --this->cursor;
    unsigned char marker = this->readNumFromFile(1);
    cout << "Find marker " << strmarker(marker, this->cursor - 2) << " !\n";
    unsigned short length = this->findLength();
    this->cursor += length;
}

void JPEG::markerSOI()
{
    cout << "Find marker \"Start of Image\" " << strmarker(0xd8, this->cursor - 2) << " !\n";
}

void JPEG::markerEOI()
{
    cout << "Find marker \"End of Image\" " << strmarker(0xd9, this->cursor - 2) << " !\n";
}

void JPEG::markerDRI()
{
    cout << "Find marker \"Define Restart Interval\" " << strmarker(0xdd, this->cursor - 2) << " !\n";
    unsigned short length = this->findLength();
    this->restartinterval = this->readNumFromFile(2);
    cout << "\tRestart Interval: " << restartinterval<<'\n';
}

void JPEG::markerSOF()
{
    cout << "Find marker \"Start of Frame\" " << strmarker(0xc0, this->cursor - 2) << " !\n";
    unsigned short length = this->findLength();
    this->sampleprecision = this->readNumFromFile(1);
    cout << "\tSample Precision: "<< (unsigned short) this->sampleprecision << '\n';
    this->height = this->readNumFromFile(2);
    this->width = this->readNumFromFile(2);
    cout << "\tHeight: " << this->height << "\n\tWidth :" << this->width << '\n';
    unsigned char components = this->readNumFromFile(1);
    for(unsigned char i = 0; i < components; ++i)
    {
        Component * c = new Component;
        c->id = this->readNumFromFile(1);
        c->sampheigth = this->readNumFromFile(1);
        c->sampwidth = c->sampheigth >> 4;
        c->sampheigth &= 0x0F;
        c->qtnum = this->readNumFromFile(1);
        cout << "Component " << JPEG::compnames[c->id] << ":\n";
        cout << "\t\tID: " << (unsigned short) c->id << '\n';
        cout << "\t\tSample ver.: " << (unsigned short) c->sampheigth << '\n';
        cout << "\t\tSample hor.: " << (unsigned short) c->sampwidth << '\n';
        cout << "\t\tNum of qtable: " << (unsigned short) c->qtnum <<'\n';
        this->components.push_back(c);
    }
}

string huffmanarr(unsigned char * counts, unsigned char hlength, unsigned char * symbols)
{
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

void JPEG::markerDHT()
{
    cout << "Find marker \"Define Huffman Table\" " << strmarker(0xc4, this->cursor - 2) << " !\n";
    unsigned short length = this->findLength();
    unsigned long long endcursor = this->cursor + length;
    unsigned const char hlength = 16;
    while (this->cursor != endcursor)
    {
        unsigned char num = this->readNumFromFile(1);
        unsigned char *counts = new unsigned char[hlength];
        unsigned short sum = 0;
        for (unsigned char i = 0; i < hlength; ++i) {
            counts[i] = this->readNumFromFile(1);
            sum += counts[i];
        }
        unsigned char *symbols = new unsigned char[sum];
        for (unsigned short i = 0; i < sum; ++i) {
            symbols[i] = this->readNumFromFile(1);
        }
        Huffman *h = new Huffman(hlength);
        h->createFromJPEG(counts, symbols);
        if (num >> 4) {
            num &= 0x0F;
            cout << "\tType: AC\n\tNumber: " << (unsigned short)num << '\n';
            this->AC.emplace(num, h);
        } else {
            num &= 0x0F;
            cout << "\tType: DC\n\tNumber: " << (unsigned short)num << '\n';
            this->DC.emplace(num, h);
        }
        cout << "Array: " << huffmanarr(counts, hlength, symbols) << '\n';
        delete[] symbols;
        delete[] counts;
    }
}

void JPEG::markerDQT()
{
    cout << "Find marker \"Define Quantization Table\" " << strmarker(0xdb, this->cursor - 2) << " !\n";
    unsigned short length = this->findLength();
    unsigned long long endcursor = this->cursor + length;
    while (this->cursor != endcursor)
    {
        unsigned char qtinform = this->readNumFromFile(1);
        unsigned char qtprec = (qtinform >> 1) ? 16: 8;
        qtinform &= 0x0F;
        this->qtprec.emplace(qtinform, qtprec);
        unsigned short tempsize = qtprec;
        tempsize *= tempsize;
        unsigned char * temp = new unsigned char [qtprec * qtprec];
        cout << "\tNumber: " << (unsigned short) qtinform << '\n';
        cout << "\tPrecision: " << (unsigned short) qtprec << '\n';
        for(unsigned short i = 0; i < tempsize; ++i) temp[i] = this->readNumFromFile(1);
        unsigned char ** table = new unsigned char * [qtprec];
        for(unsigned char i = 0; i < qtprec; ++i) table[i] = new unsigned char [qtprec];
        cout << "\tTable:\n";
        for(unsigned char i = 0; i< qtprec; ++i)
        {
            cout << "\t\t";
            for(unsigned char j = 0; j < qtprec; ++j)
            {
                table[i][j] = temp[JPEG::ZIGZAG[i][j]];
                cout << (unsigned short) table[i][j] << '\t';
            }
            cout << '\n';
        }
        this->QT.emplace(qtinform, table);
        delete [] temp;
    }
}

void JPEG::markerSOS()
{
    cout << "Find marker \"Start of Scan\" " << strmarker(0xda, this->cursor - 2) << " !\n";
    unsigned short length = this->readNumFromFile(2);
    cout << "\tLength: " << length << '\n';
    unsigned char comps = this->readNumFromFile(1);
    for(unsigned char i = 0; i < comps; ++i)
    {
        unsigned char id = this->readNumFromFile(1);
        unsigned char ACnum = this->readNumFromFile(1);
        unsigned char DCnum = ACnum >> 4;
        ACnum &= 0x0F;
        unsigned char j = 0;
        for(; j < this->components.size(); ++j) if (this->components[j]->id == id) break;
        if (i != j)
        {
            Component * c = this->components[i];
            this->components[i] = this->components[j];
            this->components[j] = c;
        }
        this->components[i]->ACnum = ACnum;
        this->components[i]->DCnum = DCnum;
        cout << "\tComponent " << JPEG::compnames[this->components[i]->id] <<":\n";
        cout << "\t\tNumber of AC: " <<(unsigned short) this->components[i]->ACnum << '\n';
        cout << "\t\tNumber of DC: " <<(unsigned short) this->components[i]->DCnum << '\n';
    }
    this->cursor += 3;
    this->datacursor = this->cursor;
    stringstream temp;
    temp << "0x" << hex << setfill('0') << setw(8) << this->datacursor;
    cout << "\tData addr: " << temp.str() << '\n';
}

void JPEG::saveClearJpeg()
{
    this->genStats();
    map<unsigned char, Huffman *> DC, AC;
    map<unsigned char, map<unsigned char, unsigned long long>> DCstat, ACstat;
    for(auto key = this->components.begin(); key != this->components.end(); ++key)
    {
        if (DCstat.find((*key)->DCnum) == DCstat.end())
        {
            DCstat.emplace((*key)->DCnum, map<unsigned char, unsigned long long>());
        }
        map<unsigned char, unsigned long long> * dc = &DCstat[(*key)->DCnum];
        for(auto jey = this->stats[(*key)->id][false].begin(); jey != this->stats[(*key)->id][false].end(); ++jey)
        {
            if (dc->find(jey->first) == dc->end()) dc->emplace(jey->first, jey->second);
            else (*dc)[jey->first] += jey->second;
        }

        if (ACstat.find((*key)->ACnum) == ACstat.end())
        {
            ACstat.emplace((*key)->ACnum, map<unsigned char, unsigned long long>());
        }
        map<unsigned char, unsigned long long> * ac = &ACstat[(*key)->ACnum];
        for(auto jey = this->stats[(*key)->id][true].begin(); jey != this->stats[(*key)->id][true].end(); ++jey)
        {
            if (ac->find(jey->first) == ac->end()) ac->emplace(jey->first, jey->second);
            else (*ac)[jey->first] += jey->second;
        }
    }
    for(auto key = DCstat.begin(); key != DCstat.end(); ++key)
    {
        cout << "DC\n" << flush;
        Huffman * dc = new Huffman(16);
        dc->createFromFrequencies(DCstat[key->first]);
        DC.emplace(key->first, dc);
        cout << "AC\n" << flush;
        Huffman * ac = new Huffman(16);
        auto temp = ACstat[key->first];
        ac->createFromFrequencies(temp);
        AC.emplace(key->first, ac);
    }
    for(auto key = this->DC.begin(); key !=this->DC.end(); ++key)
    {
        cout << "OLD " << (unsigned short) key->first << '\n';
        cout << this->DC[key->first]->showData() << '\n';
        cout << "NEW " << (unsigned short) key->first << '\n';
        cout << DC[key->first]->showData() << '\n';
    }
    for(auto key = this->AC.begin(); key !=this->AC.end(); ++key)
    {
        cout << "OLD " << (unsigned short) key->first << '\n';
        cout << this->AC[key->first]->showData() << '\n';
        cout << "NEW " << (unsigned short) key->first << '\n';
        cout << AC[key->first]->showData() << '\n';
    }
    this->stats.clear();
    /*string foutfilename = this->fin->getFileName();
    foutfilename = foutfilename + ".clear.jpg";
    ofstream fout(foutfilename, ios::binary|ios::out);
    for(unsigned long long  i = 0; i < this->markersaddr.size() - 1; ++i)
    {
        if (binary_search(JPEG::clearMarkers, JPEG::clearMarkers + 7, this->markerstype[i]))
        {
            stringstream  temp;
            temp << "0x" << hex << setfill('0') << setw(2) << (unsigned short)this->markerstype[i];
            cout << "Write Marker \"" << temp.str() << "\" !\n";
            unsigned long long addrstart = this->markersaddr[i];
            unsigned long long length = this->markersaddr[i + 1] - addrstart;
            unsigned char * c = this->fin->readBytes(addrstart, length);
            fout.write((char *) c, length);
            delete [] c;
        }
    }
    */
    //cout << "FILE SAVED TO \"" << foutfilename <<"\"\n";
}

string strbitbuffer(unsigned long long buffer, unsigned short bitlength)
{
    stringstream temp;
    if (bitlength)
    {
        for (unsigned long long bit = (1 << (bitlength - 1)); bit != 0; bit >>= 1) {
            temp << bool(buffer & bit);
        }
    }
    return temp.str();
}

signed long **JPEG::getNextTable()
{
    Component * comp = this->components[this->ccid];
    signed long ** table = new signed long * [this->sampleprecision];
    for(unsigned char i = 0; i < this->sampleprecision; ++i)
    {
        table[i] = new signed long[this->sampleprecision];
        fill(table[i], table[i] + this->sampleprecision, 0);
    }
    signed long * mass = new signed long [this->sampleprecision * this->sampleprecision];
    fill(mass, mass + this->sampleprecision * this->sampleprecision, 0);

    Huffman * code = this->DC[comp->DCnum];
    unsigned char sym, category, nulls;
    signed long long num = 0;

    //cout << "Buffer: " << strbitbuffer(this->buffer, this->bitlength) << '\n';
    while(!code->decode(this->buffer, this->bitlength, category))
    {
        this->extendBuffer();
        //cout << "Buffer: " << strbitbuffer(this->buffer, this->bitlength) << '\n';
    }
    //cout << "Buffer: " << strbitbuffer(this->buffer, this->bitlength) << '\n';
    while(!Huffman::decodeCategory(category, this->buffer, this->bitlength, num))
    {
        this->extendBuffer();
        //cout << "Buffer: " << strbitbuffer(this->buffer, this->bitlength) << '\n';
    }
    //cout << "Buffer: " << strbitbuffer(this->buffer, this->bitlength) << '\n';
    comp->DC += num;
    mass[0] = comp->DC;

    for(unsigned char i = 1; i < this->sampleprecision*this->sampleprecision; ++i)
    {
        code = this->AC[comp->ACnum];
        while(!code->decode(this->buffer, this->bitlength, sym))
        {
            this->extendBuffer();
            //cout << "Buffer: " << strbitbuffer(this->buffer, this->bitlength) << '\n';
        }
        //cout << "Buffer: " << strbitbuffer(this->buffer, this->bitlength) << '\n';
        if (!sym) break;
        nulls = sym >> 4;
        while(nulls != 0)
        {
            ++i;
            --nulls;
        }
        category = sym & 0x0F;
        while(!Huffman::decodeCategory(category, this->buffer, this->bitlength, num))
        {
            this->extendBuffer();
            //cout << "Buffer: " << strbitbuffer(this->buffer, this->bitlength) << '\n';
        }
        //cout << "Buffer: " << strbitbuffer(this->buffer, this->bitlength) << '\n';
        mass[i] = num;
    }

    for(unsigned char i = 0; i < this->sampleprecision; ++i)
    {
        for(unsigned char j = 0; j < this->sampleprecision; ++j)
        {
            table[i][j] = mass[JPEG::ZIGZAG[i][j]];
        }
    }
    delete [] mass;
    return table;
}

void JPEG::decodeTables()
{
    this->decodeStart();
    map<unsigned char, ofstream *> foutfiles;
    for(auto key = this->components.begin(); key != this->components.end(); ++key)
    {
        string foutfilename = this->fin->getFileName() + "."+ JPEG::compnames[(*key)->id] + ".tables";
        ofstream * fout = new ofstream (foutfilename, ios::out);
        foutfiles.emplace((*key)->id, fout);

    }
    while (this->mcunum != this->numberofmcu)
    {
        ofstream *fout = foutfiles[this->components[this->ccid]->id];
        signed long **table = this->getNextTable();
        this->step();
        for(unsigned char i = 0; i < this->sampleprecision; ++i)
        {
            unsigned char sp1 = this->sampleprecision - 1;
            for(unsigned char j = 0; j < sp1; ++j)
            {
                (*fout) << table[i][j] << ", ";
            }
            (*fout) << table[i][sp1] << "\n";
        }
        *fout << "\n";
        for(unsigned char i = 0; i < this->sampleprecision; ++i)
        {
            delete [] table[i];
        }
        delete [] table;
    }
    cout << "\n";
    for(auto key = foutfiles.begin(); key != foutfiles.end(); ++key)
    {
        string foutfilename = this->fin->getFileName() + "."+ JPEG::compnames[(*key).first] + ".tables";
        cout << "File \"" << foutfilename << "\" saved!\n";
        key->second->close();
        delete key->second;
    }
}

void JPEG::extendBuffer()
{
    if ((64 - this->bitlength) > this->sampleprecision)
    {
        unsigned char c = this->readNumFromFile(1);
        if (c == 0xFF) this->readNumFromFile(1);
        this->buffer = (this->buffer << this->sampleprecision) | c;
        this->bitlength += this->sampleprecision;
    }
}

void JPEG::genMCUNumber()
{
    this->hsample = 0;
    this->wsample = 0;
    for(auto key = this->components.begin(); key != this->components.end(); ++key)
    {
        if ((*key)->sampwidth > this->wsample) this->wsample = (*key)->sampwidth;
        if ((*key)->sampheigth > this->hsample) this->hsample = (*key)->sampheigth;
    }
    this->hsample *= this->sampleprecision;
    this->wsample *= this->sampleprecision;
    this->mcuw = (this->width / this->wsample) + ((this->width % this->wsample) ? 1: 0);
    this->mcuh = (this->height / this->hsample) + ((this->height % this->hsample) ? 1: 0);
    this->numberofmcu = this->mcuw * this->mcuh;
}

void JPEG::decodeStart()
{
    this->findMarkers();
    this->cursor = datacursor;
    this->mcunum = 0;
    this->ccid = 0;
    this->hs = 0;
    this->ws = 0;
    this->buffer = 0;
    this->bitlength = 0;
    this->genMCUNumber();
    cout << "Number of MCU: " << this->numberofmcu << '\n';
}

void JPEG::step()
{
    Component * comp = this->components[this->ccid];
    ++this->ws;
    if (comp->sampwidth == this->ws)
    {
        this->ws = 0;
        ++this->hs;
        if (comp->sampheigth == this->hs)
        {
            this->hs = 0;
            ++this->ccid;
            if (this->ccid == this->components.size())
            {
                this->ccid = 0;
                ++this->mcunum;
                if (!(this->mcunum % this->restartinterval))
                {
                    unsigned long num = this->readNumFromFile(2);
                    //cerr << hex << num << '\n' << flush;
                    for(auto key = this->components.begin(); key != this->components.end(); ++key)
                    {
                        (*key)->DC = 0;
                    }
                    this->buffer = 0;
                    this->bitlength = 0;
                }
            }
        }
    }
}

void JPEG::getStats()
{
    this->genStats();
    string foutfilename = this->fin->getFileName();
    foutfilename = foutfilename + ".stats";
    ofstream fout(foutfilename, ios::out);
    for(auto key = this->components.begin(); key != this->components.end(); ++key)
    {
        map<unsigned char, unsigned long long> * table = &this->stats[(*key)->id][false];
        fout << JPEG::compnames[(*key)->id] << ' ';
        fout << "DC\n{";
        bool ok = false;
        for(auto key = table->begin(); key != table->end(); ++key)
        {
            if (ok)  fout <<", ";
            ok = true;
            fout << (unsigned short) key->first << ": " << key->second;
        }
        fout << "}\n";
        table = &this->stats[(*key)->id][true];
        fout << JPEG::compnames[(*key)->id] << ' ' << "AC\n{";
        ok = false;
        for(auto key = table->begin(); key != table->end(); ++key)
        {
            if (ok)  fout <<", ";
            ok = true;
            fout << (unsigned short) key->first << ": " << key->second;
        }
        fout << "}\n";
    }
    fout.close();
    this->stats.clear();
}

void JPEG::genStats()
{
    //map<unsigned char, map <bool, map<unsigned char, unsigned long>>>
    this->decodeStart();
    map<unsigned char, signed long> DC;
    for(auto key = this->components.begin(); key != this->components.end(); ++key)
    {
        DC.emplace((*key)->id, 0);
        this->stats.emplace((*key)->id, map <bool, map<unsigned char, unsigned long long>>());
        this->stats[(*key)->id].emplace(true, map<unsigned char, unsigned long long>()); // AC
        this->stats[(*key)->id].emplace(false, map<unsigned char, unsigned long long>()); // DC
    }
    while (this->mcunum != this->numberofmcu)
    {
        signed long ** table = this->getNextTable();
        unsigned short size = this->sampleprecision * this->sampleprecision;
        signed long * mass = new signed long [size];
        for(unsigned char i = 0; i < this->sampleprecision; ++i)
        {
            for(unsigned char j = 0; j < this->sampleprecision; ++j)
            {
                mass[JPEG::ZIGZAG[i][j]] = table[i][j];
            }
            delete [] table[i];
        }
        delete [] table;
        map<unsigned char, unsigned long long> * temp = &this->stats[this->components[this->ccid]->id][false];
        unsigned char category = JPEG::getCategory(DC[this->components[this->ccid]->id] - mass[0]);
        if (temp->find(category) == temp->end()) temp->emplace(category, 0);
        (*temp)[category] = (*temp)[category] + 1;
        DC[this->components[this->ccid]->id] = mass[0];
        unsigned short nulls = size - 1;
        temp = &this->stats[this->components[this->ccid]->id][true];
        while ((!mass[nulls]) && (nulls != 0)) --nulls;
        if (nulls != size - 1)
        {
            size = nulls + 1;
            category = 0;
            if (temp->find(category) == temp->end()) temp->emplace(category, 0);
            ++(*temp)[category];
        }
        nulls = 0;
        for(unsigned short i = 1; i < size; ++i)
        {
            if ((mass[i]) || (nulls == 15))
            {
                category = JPEG::getCategory(mass[i]) | (nulls << 4);
                if (temp->find(category) == temp->end()) temp->emplace(category, 0);
                ++(*temp)[category];
                nulls = 0;
            }
            else ++nulls;
        }
        delete [] mass;
        this->step();
    }
}

unsigned char JPEG::getCategory(signed long a)
{
    if (a)
    {
        signed long b = 1;
        unsigned char i = 0;
        while ((a >= b) || (a <= -b))
        {
            b <<= 1;
            ++i;
        }
        return i;
    }
    else return 0;
}



