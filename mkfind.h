#ifndef MKFIND_H
#define MKFIND_H

#include <string_view>
#include <string>
#include <fstream>
#include <list>

struct Match
{
    std::size_t line;
    std::size_t pos;
    std::string str;
};

/*
 * Finds substrings in 'file' with 'mask'.
 */
std::list<Match> mkfind(std::ifstream& file, std::string_view mask);

#endif // MKFIND_H
