#include "JPEG/JPEG.h"

int main (int argc,  char ** args)
{
    if (argc < 3)
    {
        cerr << "Use: jpegcompress.exe [codingtable file] [jpeg file]\n";
        raise(3);
    }
    else
    {
        JPEG photo(args[2]);
        photo.compressJPEG(args[1]);
    }
}
