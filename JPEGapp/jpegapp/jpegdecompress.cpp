#include "JPEG/JPEG.h"

int main (int argc,  char ** args)
{
    if (argc < 3)
    {
        cerr << "Use: jpegdecompress.exe [codingtable file] [jpeg file]\n";
        raise(3);
    }
    else
    {
        JPEG photo(args[2]);
        photo.decompressJPEG(args[1]);
    }
}
