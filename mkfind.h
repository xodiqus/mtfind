#ifndef MKFIND_H
#define MKFIND_H

#include <string_view>
#include <string>
#include <fstream>
#include <list>
#include <vector>

struct Result
{
    std::size_t line;

    struct Data {
        std::size_t pos;
        std::string str;
    };

    std::vector<Data> values;
};

std::list<Result> mkfind(std::ifstream&, std::string_view mask);

#endif // MKFIND_H
