#ifndef MKFIND_H
#define MKFIND_H

#include <string_view>
#include <string>
#include <deque>
#include <functional>

namespace detail {

struct Result
{
    std::size_t line;

    struct Data {
        std::size_t pos;
        std::string str;
    };

    std::deque<Data> values;
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
std::deque<Match> mkfind(std::function<bool(std::string*)> stringReader, std::string_view mask);

#endif // MKFIND_H
