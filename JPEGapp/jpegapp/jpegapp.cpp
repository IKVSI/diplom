#include <iostream>
#include "JPEG/ReadFile.h"


int main()
{
    ReadFile A("../../Scripts/test1.jpg");
    std::cout << A.getStrBuffer();
    return 0;
}


