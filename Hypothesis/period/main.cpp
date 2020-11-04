#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <cmath>
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
        //"X1&X2&X3&X4&X6&X7&X8 V -X5",
        //"-X1&-X2&-X3&-X4&-X6&-X7&-X8 V X5"
//"-x1 V x2", // 8
//"-x2&-x3 V x1" // 16
"x2 V -x1-x3-x4" // 32
/*"x1 V -x2-x3-x4-x5", // 64
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
        //"x1x2 V x3x4 V x5x6 V -x7-x8",
//"x1 V -x2&-x3" //8
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

struct duple
{
    std::vector<std::int16_t> seq;
    std::vector<std::int16_t> base;
};
std::vector<std::vector<std::int16_t>> permutations(std::int16_t &size)
{
    duple target;
    std::vector<std::vector<std::int16_t>> out;
    for(std::int16_t i = size; i > 0; --i) target.base.push_back(i);
    target.seq.push_back(0);
    while(target.seq.front() != -1)
    {
        //std::cout << "Sequence: " << target.seq << "\nBase: " << target.base << '\n';
        if (target.seq.back())
        {
            target.seq.pop_back();
            std::int16_t last = target.seq.back();
            target.seq.pop_back();
            auto it = target.base.begin();
            while((it != target.base.end()) && (*it > last)) ++it;
            if (it == target.base.begin())
            {
                target.base.insert(it, last);
                target.seq.push_back(-1);
            }
            else
            {
                it = target.base.insert(it, last);
                --it;
                target.seq.push_back(*it);
                target.base.erase(it);
                target.seq.push_back(0);
            }
        }
        else
        {
            target.seq.pop_back();
            if (target.base.empty())
            {
                out.push_back(target.seq);
                target.seq.push_back(-1);
            } else {
                target.seq.push_back(target.base.back());
                target.base.pop_back();
                target.seq.push_back(0);
            }
        }
    }
    return out;
}

std::vector<std::vector<std::int16_t>> generate(std::vector<std::int16_t> &part, std::vector<std::int16_t> &perm)
{
    std::vector<std::vector<std::int16_t>> out;
    bool neok = false;
    std::uint32_t k = 0;
    std::uint16_t lastblocksize = 0;
    std::int16_t lastfirst = 0;
    std::int16_t last = 0;
    for(auto i = part.begin(); i != part.end(); ++i)
    {
        std::vector<std::int16_t> temp;
        while (temp.size() != *i)
        {
            if ((lastblocksize == *i) && (lastfirst > perm[k])) { neok = true; break; }
            else if (last > perm[k]) { neok = true; break; }
            temp.push_back(perm[k]);
            last = perm[k];
            ++k;
        }
        if (neok) break;
        out.push_back(temp);
        last = 0;
        lastfirst = temp.front();
        lastblocksize = *i;
    }
    if (neok) out.clear();
    return out;
}

void genTest(std::int16_t size)
{
    std::vector<std::vector<std::int16_t>>parts = partitions(size);
    std::cerr<<"End start generation parts!\n"<<std::flush;
    std::vector<std::vector<std::int16_t>> perms = permutations(size);
    std::cerr<<"End start generation perms!\n"<<std::flush;
    std::uint64_t all = parts.size()*perms.size();
    std::int64_t k = 1;
    std::int64_t p = 1;
    std::int64_t pn = 1;
    std::int64_t period = pow(2, size+1);
    for(auto part = parts.begin(); part != parts.end(); ++part)
    {
        for(auto perm = perms.begin(); perm != perms.end(); ++perm)
        {
            std::vector<std::vector<std::int16_t>> generation = generate(*part, *perm);
            if (generation.size())
            {
                std::int16_t n = 1;
                std::uint16_t lastsize = 0;
                std::cerr<<k<<'/'<<all<<'\n' << std::flush;
                for(auto i = generation.begin(); i != generation.end(); ++i)
                {
                    for(uint32_t j = 0; j < (*i).size(); ++j) (*i)[j] = -(*i)[j];
                    auto funcG = Function(generation);
                    if (period == funcG.getPeriod())
                    {
                        std::cout<<p<<'/'<<n<<'/'<<k<<'/'<<all<<": "<<funcG.getFunction()<<'\n';
                        std::cout<< "Period: " << funcG.getPeriod()<<'\n';
                    }
                    for(uint32_t j = 0; j < (*i).size(); ++j) (*i)[j] = -(*i)[j];
                    ++n;
                    ++pn;
                }
                ++p;
            }
            ++k;
        }
    }
    std::cerr<<k-1<<'/'<<all<<'\n' << std::flush;
    std::cout<<"real: " << pn << " realnegless: " << p << " allnegless: " << all <<'\n';
}

void genTestOne(std::int16_t size)
{
    std::uint32_t period = pow(2, size+1);
    std::uint32_t mxperiod = 0;
    Function * mxfuncG;
    bool ok = true;
    std::vector<std::int16_t> temp, retemp;
    for(std::int16_t i = 1; i <= size; ++i)
    {
        temp.emplace_back(i);
        retemp.emplace_back();
    }

    for(auto i = temp.begin(); i != temp.end(); ++i)
    {
        std::cerr << (*i)-1 << '/' << temp.size()<<'\n' << std::flush;
        std::vector<std::vector<std::int16_t>> generation;
        std::vector<std::int16_t> a;
        a.push_back(-(*i));
        auto j = temp.erase(i);
        generation.push_back(temp);
        generation.push_back(a);
        auto * funcG = new Function(generation);
        std::cout<<"Period: "<<funcG->getPeriod()<<'\n';
        delete funcG;
        i = temp.insert(j, -a[0]);
    }
    std::cerr << temp.size() << '/' << temp.size()<<'\n' << std::flush;
}

int main(int argc, char* argv[])
{
    std::string s(argv[1]);
    genTest(std::stoi(s));
    /*std::cout<<"n = "<< 15 << ": ";
    genTestOne(20);
    std::cout<<'\n';*/
    /*for(std::uint32_t i = 2; i < 24; ++i)
    {
        std::cout<<"n = "<< i+1 << ": ";
        genTestOne(i);
        std::cout<<'\n';
    }*/
    /*for(auto i = TEST.begin(); i != TEST.end(); ++i)
    {
        auto funcG = Function(*i);
        std::cout<<funcG;
    }*/
    return 0;
}
