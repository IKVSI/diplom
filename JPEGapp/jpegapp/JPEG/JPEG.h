
#ifndef JPEGAPP_JPEG_H
#define JPEGAPP_JPEG_H

#include "ReadFile.h"
#include <algorithm>
#include <csignal>
#include <vector>
using namespace std;


class JPEG
{
    typedef void (JPEG::*jpegfunc)();
private:
    vector<unsigned char> markerstype;
    vector<unsigned long long int> markersaddr;
    bool startimage = false;
    bool endimage = false;
    unsigned long long int readlength = 1;
    unsigned long long int cursor = 0;
    void defaultMarker();
    void ignoreMarker();
    void SOIMarker();
    void EOIMarker();
    bool isMarker();
    void runMarker();
    void findMarkers();
    ReadFile * fin;
public:
    JPEG(string filename);
    ~JPEG();
    jpegfunc MARKERS[256];
    void saveClearJpeg();
};


#endif //JPEGAPP_JPEG_H
