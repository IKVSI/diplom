#include <iostream>
#include <string>
#include <vector>
#include <set>
#include "Function.h"

#include <iostream>
template <typename T>
std::ostream& operator<< (std::ostream& out, const std::vector<T>& v)
{
    out << "[ ";
    int i = 0;
    if (v.size())
    {
        while (i < (v.size()-1))
        {
            out << v[i] << ", ";
            ++i;
        }
        out << v[i];
    }
    out << " ]";
    return out;
}

std::vector<std::string> TEST =
        {
"-x1 V x2", // 8
"x1 V -x2-x3", // 16
/*"x2 V -x1-x3-x4", // 32
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
"x2 V x6 V -x5-x4-x3-x1",*/
"x1 V -x2" //8
        };

std::vector<std::vector<std::int16_t>> partitions(std::int16_t &size)
{
    std::vector<std::vector<std::int16_t>> parts;
    std::vector<std::vector<std::int16_t>> queue;
    queue.emplace_back();
    queue.back().emplace_back(size);
    std::uint32_t i = 0;
    while (!queue.empty())
    {
        std::vector<std::int16_t> temp = queue.back();
        queue.pop_back();
        std::int16_t cursize = temp.back();
        temp.pop_back();
        std::int16_t minimum = (temp.empty() || (cursize < temp.back())) ? cursize: temp.back();
        if (cursize)
        {
            for(std::int16_t i = 1; i <= minimum; ++i)
            {
                temp.push_back(i);
                temp.push_back(cursize - i);
                queue.push_back(temp);
                temp.pop_back();
                temp.pop_back();
            }
        }
        else parts.push_back(temp);
    }
    return parts;
}


std::vector<std::vector<std::vector<std::int16_t>>> permutations(std::vector<std::int16_t> &part, std::int16_t &size)
{
    std::set<std::int16_t> base;
    for(std::int16_t i = 1; i <= size; ++i) base.insert(i);
    std::vector< std::tuple< std::vector< std::vector<std::int16_t>>, std::set<std::int16_t>>> queue;
    std::tuple< std::vector< std::vector<std::int16_t>>, std::set<std::int16_t>> temp(std::vector< std::vector<std::int16_t>>(), base);
    queue.push_back(temp);
    while (!queue.empty())
    {
        std::tuple< std::vector< std::vector<std::int16_t>>, std::set<std::int16_t>> temp = queue.back();
        queue.pop_back();

    }
    std::vector<std::vector<std::vector<std::int16_t>>> out;
    return out;
}

void genTest(std::int16_t size)
{
    std::vector<std::vector<std::int16_t>>parts = partitions(size);
    for(std::uint32_t i = 0; i < parts.size(); ++i)
    {
        std::set<std::int16_t> ne(parts[i].begin(), parts[i].end());
        std::vector<std::vector<std::vector<std::int16_t>>> perm = permutations(parts[i], size);
        /*for(auto j = ne.begin(); j != ne.end(); ++j)
        {

        }*/
    }
}

int main()
{
    genTest(5);
    /*for(auto i = TEST.begin(); i != TEST.end(); ++i)
    {
        auto funcG = Function(*i);
        std::cout << funcG << '\n';
    }*/
    return 0;
}
