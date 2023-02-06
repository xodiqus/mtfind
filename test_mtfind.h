#ifndef TEST_MTFIND_H
#define TEST_MTFIND_H

#include <vector>

#include "mtfind.h"

namespace detail {

Result parse(std::string_view in, std::string_view mask);

std::tuple<std::vector<std::string_view>, std::size_t> divide_string(std::string_view in, std::size_t mask_length, std::size_t top_limit);

}

#endif // TEST_MTFIND_H
