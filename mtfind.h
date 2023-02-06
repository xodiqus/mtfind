#ifndef MKFIND_H
#define MKFIND_H

#include <string_view>
#include <string>
#include <list>
#include <functional>

namespace detail {

struct Result
{
    std::size_t line;

    struct Data {
        std::size_t pos;
        std::string str;
    };

    std::list<Data> values;
};

}

struct Match
{
    std::size_t line;
    std::size_t pos;
    std::string str;
};

/*
 * Finds substrings using 'stringReader' with 'mask'.
 */
std::list<Match> mkfind(std::function<bool(std::string*)> stringReader, std::string_view mask);

#endif // MKFIND_H
