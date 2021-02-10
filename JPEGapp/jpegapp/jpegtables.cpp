#include "JPEG/JPEG.h"

int main (int argc,  char ** args)
{
    if (argc < 2)
    {
        cerr << "Use: jpegtables.exe [jpeg file]\n";
        throw 3;
    }
    else
    {
        try
        {
            JPEG photo(args[1]);
            photo.decodeTables();
        }
        catch (int a)
        {
            cout << flush;
            cerr << "Error: " << a << '\n';
            throw a;
        }
    }
}