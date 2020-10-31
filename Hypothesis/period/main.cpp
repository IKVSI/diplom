#include <iostream>
#include <string>
#include <vector>
#include "Function.h"

std::vector<std::string> TEST =
        {
/*"-x1 V x2", // 8
"x1 V -x2-x3", // 16
"x2 V -x1-x3-x4", // 32
"x1 V -x2-x3-x4-x5", // 64
"x1 V -x2-x3-x4-x5-x6", // 128
"x1 V x2x3 V -x4", // 32
"x1 V x2x3 V -x4-x5", // 64
"x1 V x2x4x6 V -x3-x5-x7", // 231
"x1x2 V -x3-x4", // 32
"x1x2 V x3x4 V x5x6 V -x7-x8", // 502
"x1x2 V x3x4 V x5x6 V -x7", // 216
"x1 V x2 V -x3", // 16
"x1x3 V x2x4 V x7x5 V -x6-x8",
"-x1-x2-x3 V x4 V x5 V x6x7x8",
"x1 V x3x5 V -x2-x4-x6", //128
"x2 V x6 V -x5-x4-x3-x1",
"x1 V -x2"*/ //8
        };



void genTest()
{

}

int main()
{
    genTest();
    /*for(auto i = TEST.begin(); i != TEST.end(); ++i)
    {
        auto funcG = Function(*i);
        std::cout << funcG << '\n';
    }*/
    return 0;
}
