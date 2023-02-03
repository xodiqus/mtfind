#include <iostream>
#include <sstream>

#include "mkfind.h"

int main(int argc, char** argv)
{
    if (argc != 3) {
        std::cerr << "Requires two arguments! Synopsis: " << std::endl
                  <<  "	mtfind file.txt \"mask\"" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream file(argv[1]);
    const std::string_view mask = argv[2];

    if (!file.is_open()) {
        std::cerr << "Can't open: " << argv[1] << "!" << std::endl;
    }

    const auto results = mkfind(file, mask);
    std::stringstream ss;
    std::size_t i = 0;

    for (auto && result: results)
    {
        for (auto && [pos, str]: result.values)
        {
            ++i;
            ss << result.line+1 << ' ' << pos+1 << ' ' << str << std::endl;
        }
    }

    std::cout << i << std::endl
              << ss.str();

    return EXIT_SUCCESS;
}