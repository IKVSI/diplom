#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <map>

#ifndef PERIOD_FUNCTION_H
#define PERIOD_FUNCTION_H


class Function {
public:
    Function(std::string stringform);
    Function(std::vector<std::vector<std::int16_t>> &generation);
    std::vector<std::vector<std::int16_t>> getGeneration();
    std::int16_t getSize();
    std::uint8_t getValue(std::vector<std::uint8_t> &temp);
    std::uint32_t getPeriod();
    std::string getTable();
    friend std::ostream& operator<< (std::ostream& out, const Function& other);
private:
    std::int16_t size = 0;
    std::vector<std::vector<std::int16_t>> generation;
    std::map<std::vector<std::uint8_t>, std::uint8_t> table;
    std::vector<std::uint8_t> sequence;
    void generationfromstring(std::string &stringform);
    void generatePeriodTable();
};


#endif //PERIOD_FUNCTION_H
