#ifndef MKFIND_H
#define MKFIND_H

#include <string_view>
#include <string>
#include <deque>
#include <functional>

namespace mtfind {

struct Result
{
    std::size_t line;

    struct Data {
        std::size_t pos;
        std::string str;
    };

    std::deque<Data> values;
};

struct Match
{
    std::size_t line;
    std::size_t pos;
    std::string str;
};

Result find_bf(std::string_view in, std::string_view mask);

using Finder = std::function<Result(std::string_view, std::string_view)>;
using StringReader = std::function<bool(std::string*)>;

/*
 * Finds substrings using 'stringReader' with 'mask'.
 */
std::deque<Match> mtfind(StringReader stringReader, std::string_view mask, Finder = find_bf);

}

#endif // MKFIND_H
