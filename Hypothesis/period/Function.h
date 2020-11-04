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
    explicit Function(std::string stringform);
    explicit Function(std::vector<std::vector<std::int16_t>> generation);
    bool isFullPeriod();
    std::vector<std::vector<std::int16_t>> getGeneration();
    std::int16_t getSize();
    std::uint8_t getValue(std::vector<std::uint8_t> &temp);
    std::uint64_t getPeriod();
    std::string getTable();
    std::string getFunction();
    friend std::ostream& operator<< (std::ostream& out, const Function& other);
    ~Function();
private:
    bool is_full = false;
    std::uint64_t period = 0;
    std::int16_t size = 0;
    std::uint64_t maxsize = 1;
    std::vector<std::vector<std::int16_t>> generation;
    std::map<std::vector<std::uint8_t>, std::uint8_t> table;
    std::vector<std::uint8_t> sequence;
    void generationfromstring(std::string &stringform);
    void generatePeriodTable();
};


#endif //PERIOD_FUNCTION_H
