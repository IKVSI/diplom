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

template <typename T, typename H>
std::ostream& operator<< (std::ostream& out, const std::map<T, H>& m)
{
    out << "{\n";
    for(auto i = m.begin(); i != m.end(); ++i)
    {
        out << "    " << (*i).first << ": " << (int) (*i).second<<'\n';
    }
    out << "}\n";
    return out;
}

Function::Function(std::string stringform)
{
    stringform.erase(std::remove_if(stringform.begin(), stringform.end(), isspace), stringform.end());
    this->generationfromstring(stringform);
    this->generatePeriodTable();
}

Function::Function(std::vector<std::vector<std::int16_t>> &generation)
{

    for(auto i = generation.begin(); i != generation.end(); ++i)
    {
        std::vector<std::int16_t> temp = std::vector<std::int16_t>();
        std::copy((*i).begin(), (*i).end(), std::back_inserter(temp));
        this->generation.push_back(temp);
    }
    this->generatePeriodTable();
}


void Function::generationfromstring(std::string &stringform)
{
    std::vector<std::int16_t> temp = std::vector<std::int16_t>();
    for(std::uint32_t i = 0; i < stringform.size(); ++i)
    {
        int16_t minus = 1;
        switch(stringform[i])
        {
            case 'V':
                this->generation.push_back(temp);
                temp = std::vector<std::int16_t>();
                break;
            case '-':
                minus = -1;
                ++i;
            case 'x':
                std::uint32_t j = i + 1;
                while ((j < stringform.size()) && (stringform[j] >= '0') && (stringform[j] <= '9')) ++j;
                std::string sub = stringform.substr(i + 1, j - i - 1);
                std::int16_t subint = std::stoi(sub);
                if (this->size < subint) this->size = subint;
                temp.push_back(subint * minus);
                minus = 1;
        }
    }
    this->generation.push_back(temp);
}

std::vector<std::vector<std::int16_t>> Function::getGeneration() {
    return this->generation;
}

std::int16_t Function::getSize()
{
    return this->size;
}

void Function::generatePeriodTable()
{
    std::int16_t  size1 = size + 1;
    for (int i = 0; i < size1; ++i) this->sequence.push_back(0);
    std::vector<std::uint8_t> temp;
    auto lastsize = this->table.size() - 1;
    while (lastsize != this->table.size())
    {
        lastsize = this->table.size();
        temp = std::vector<std::uint8_t>(this->sequence.begin() + (this->sequence.size() - size1), this->sequence.end());
        std::uint8_t val = this->getValue(temp);
        this->sequence.push_back(val);
    }
}

std::uint8_t Function::getValue(std::vector<std::uint8_t> &temp)
{
    auto val = this->table.find(temp);
    if (val == this->table.end())
    {
        std::uint8_t s = 0;
        for(auto i = this->generation.begin(); i != this->generation.end(); ++i)
        {
            std::uint8_t t = 1;
            for(auto j = (*i).begin(); j != (*i).end(); ++j) t &= ((*j) < 0) ? !temp[-*j] : temp[*j];
            s |= t;
        }
        this->table[temp] = s ^ temp[0];
    }
    return this->table[temp];
}

std::uint32_t Function::getPeriod()
{
    return this->table.size();
}

std::ostream &operator<<(std::ostream &out, const Function &other)
{
    std::int16_t  size1 = other.size + 1;
    out << "Period: " << other.table.size() << " Seq: ";
    std::uint32_t i = 0;
    while(i < other.table.size())
    {
        out << (int) other.sequence[i];
        ++i;
    }
    std::uint32_t j = 0;
    while(j < other.table.size())
    {
        bool ok = true;
        for(std::uint32_t k = 0; k < size1; ++k)
        {
            if (other.sequence[j+k] != other.sequence[i+k])
            {
                ok = false;
                break;
            }
        }
        if (ok) break;
        ++j;
    }
    out << '(';
    while(j < other.table.size())
    {
        out << (int) other.sequence[j];
        ++j;
    }
    out << ')';
    return out;
}

std::string Function::getTable()
{
    std::stringstream func;
    func << "X0 + (";
    {
        std::uint32_t i = 0;
        while (i < this->generation.size() - 1) {
            std::uint32_t j = 0;
            while (j < this->generation[i].size() - 1) {
                if (this->generation[i][j] < 0) {
                    func << "-X" << -this->generation[i][j];
                } else {
                    func << "X" << this->generation[i][j];
                }
                func << "&";
                ++j;
            }
            if (this->generation[i][j] < 0) {
                func << "-X" << -this->generation[i][j];
            } else {
                func << "X" << this->generation[i][j];
            }
            func << " V ";
            ++i;
        }
        std::uint32_t j = 0;
        while (j < this->generation[i].size() - 1) {
            if (this->generation[i][j] < 0) {
                func << "-X" << -this->generation[i][j];
            } else {
                func << "X" << this->generation[i][j];
            }
            func << "&";
            ++j;
        }
        if (this->generation[i][j] < 0) {
            func << "-X" << -this->generation[i][j];
        } else {
            func << "X" << this->generation[i][j];
        }
        func << ')';
    }
    std::stringstream val;
    val << "X" << this->size;
    std::int16_t valsize = val.str().size();
    std::int32_t funcsize = func.str().size();
    std::string func2 = func.str().substr(5, func.str().size() - 5);
    std::int32_t func2size = func2.size();
    std::int16_t size1 = this->size + 1;
    std::stringstream out;
    out << '|';
    for(std::int16_t i = 0; i < size1; ++i)
    {
        std::stringstream val;
        val << "X" << i;
        out << ' ';
        for(std::int16_t i = val.str().size(); i < valsize; ++i) out << ' ';
        out << val.str() << " |";
    }
    out << ' ' << func2 << " |";
    out << ' ' << func.str() << " |";
    std::int32_t chertsize = out.str().size();
    out << '\n';
    for(std::int32_t i = 0; i < chertsize; ++i) out << '-';
    out << '\n';
    std::uint32_t i = 0;
    std::vector<std::uint8_t> target;
    for(std::int16_t i = 0; i < size1; ++i) target.push_back(0);
    while ( i < this->table.size())
    {
        if (this->table.find(target) != this->table.end())
        {
            out << '|';
            for(std::int16_t j = 0; j < size1; ++j)
            {
                out << ' ';
                std::uint16_t k = 0;
                for(;k < valsize/2; ++k) out << ' ';
                out << (int) target[j];
                for(;k < valsize; ++k) out << ' ';
                out << '|';
            }
            out << ' ';
            std::uint16_t k = 0;
            for(;k < func2size/2; ++k) out << ' ';
            out << (int) (this->table[target] ^ target[0]);
            for(;k < func2size; ++k) out << ' ';
            out << '|';
            out << ' ';
            k = 0;
            for(;k < funcsize/2; ++k) out << ' ';
            out << (int) this->table[target];
            for(;k < funcsize; ++k) out << ' ';
            out << '|';
            out << '\n';
            ++i;
        }
        for(std::int16_t j = this->size; j >= 0; --j)
        {
            if (target[j]) target[j] = 0;
            else
            {
                target[j] = 1;
                break;
            }
        }
    }
    for(std::int32_t i = 0; i < chertsize; ++i) out << '-';
    out << '\n';
    return out.str();
}

