#include <iostream>
#include <fstream>

#include "mtfind.h"

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
        return EXIT_FAILURE;
    }

    const auto results = mtfind::mtfind([&](std::string* line) { return !!std::getline(file, *line); }, mask);

    std::cout << results.size() << std::endl;

    for (auto && [line, pos, str]: results)
    {
        std::cout << line+1 << ' ' << pos+1 << ' ' << str << '\n';
    }

    return EXIT_SUCCESS;
}
