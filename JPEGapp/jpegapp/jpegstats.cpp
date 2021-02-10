#include "JPEG/JPEG.h"

int main (int argc,  char ** args)
{
    if (argc < 2)
    {
        cerr << "Use: jpegstats.exe [jpeg file]\n";
        throw 3;
    }
    else
    {
        JPEG photo(args[1]);
        try
        {
            JPEG photo(args[1]);
            photo.getStats();
        }
        catch (int a)
        {
            cout << flush;
            cerr << "Error: " << a << '\n';
            throw a;
        }
    }
}