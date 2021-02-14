#include "JPEG/JPEG.h"

int main (int argc,  char ** args)
{
    if (argc < 3)
    {
        cerr << "Use: jpegdecompress.exe [codingtable file] [jpeg file]\n";
        throw 3;
    }
    else
    {
        try
        {
            JPEG photo(args[2], true);
            photo.decompressJPEG(args[1]);
        }
        catch (int a)
        {
            cout << flush;
            cerr << "Error: " << a << '\n';
            throw a;
        }
    }
}
