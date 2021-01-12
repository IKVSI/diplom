#include <iostream>
#include "JPEG/JPEG.h"


int main(int argcount, string args)
{
    JPEG A("../../../Scripts/test2.jpg");
    A.saveClearJpeg();
    return 0;
}


