/*
 *
 */

#include "JPEG/JPEG.h"

int main (int argc,  char ** args)
{
    if (argc < 2)
    {
        cerr << "Use: jpegclear.exe [jpeg file]\n";
        raise(3);
    }
    else
    {
        JPEG photo(args[1]);
        ReadFile A(args[1]);
        unsigned long long int size = 0x45;
        unsigned char* temp = A.readBytes(0x14, size);
        delete[] temp;
    }
}