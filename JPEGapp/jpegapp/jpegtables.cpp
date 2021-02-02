#include "JPEG/JPEG.h"

int main (int argc,  char ** args)
{
    if (argc < 2)
    {
        cerr << "Use: jpegtables.exe [jpeg file]\n";
        raise(3);
    }
    else
    {
        JPEG photo(args[1]);
        photo.decodeTables();
    }
}