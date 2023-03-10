#define BOOST_TEST_MODULE mkfind_tests
#include <boost/test/included/unit_test.hpp>

#include <sstream>
#include <tuple>

#include "mtfind.h"

namespace mtfind {
    std::tuple<std::vector<std::string_view>, std::size_t> split(std::string_view in, std::size_t mask_length, std::size_t top_limit);
}

using namespace mtfind;

namespace {

auto stringReader(std::stringstream& in)
{
    return [&](std::string* line)
    {
        return !!std::getline(in, *line);
    };
}

}

BOOST_AUTO_TEST_SUITE(mkfind_tests)

BOOST_AUTO_TEST_CASE(test1)
{
    std::stringstream in;
    in << "I've paid my dues" << std::endl
       << "Time after time." << std::endl
       << "I've done my sentence" << std::endl
       << "But committed no crime." << std::endl
       << "And bad mistakes ?" << std::endl
       << "I've made a few." << std::endl
       << "I've had my share of sand kicked in my face" << std::endl
       << "But I've come through." << std::endl;


    auto result = mtfind::mtfind(stringReader(in), "?ad");

    BOOST_TEST(result.size() == 3);

    auto it = result.begin();
    if (1 <= result.size()) {
        BOOST_TEST(it->line == 4);
        BOOST_TEST(it->pos == 4);
        BOOST_TEST(it->str == "bad");
    }

    ++it;
    if (2 <= result.size()) {
        BOOST_TEST(it->line == 5);
        BOOST_TEST(it->pos == 5);
        BOOST_TEST(it->str == "mad");
    }

    ++it;
    if (3 <= result.size()) {
        BOOST_TEST(it->line == 6);
        BOOST_TEST(it->pos == 5);
        BOOST_TEST(it->str == "had");
    }
}

BOOST_AUTO_TEST_CASE(find_bf_test)
{
    auto result = find_bf("I had made", "?ad");

    BOOST_TEST(result.values.size() == 2);

    auto it = result.values.begin();
    if (1 <= result.values.size()) {
        BOOST_TEST(it->pos == 2);
        BOOST_TEST(it->str == "had");
    }

    ++it;
    if (2 <= result.values.size()) {
        BOOST_TEST(it->pos == 6);
        BOOST_TEST(it->str == "mad");
    }

    result = find_bf("You also had made", "ad?");

    BOOST_TEST(result.values.size() == 2);

    it = result.values.begin();
    if (1 <= result.values.size()) {
        BOOST_TEST(it->pos == 10);
        BOOST_TEST(it->str == "ad ");
    }

    ++it;
    if (2 <= result.values.size()) {
        BOOST_TEST(it->pos == 14);
        BOOST_TEST(it->str == "ade");
    }

    result = find_bf("abcd", "??");

    BOOST_TEST(result.values.size() == 2);

    it = result.values.begin();
    if (1 <= result.values.size()) {
        BOOST_TEST(it->pos == 0);
        BOOST_TEST(it->str == "ab");
    }

    ++it;
    if (2 <= result.values.size()) {
        BOOST_TEST(it->pos == 2);
        BOOST_TEST(it->str == "cd");
    }

    result = find_bf("ab", "?");

    BOOST_TEST(result.values.size() == 2);

    it = result.values.begin();
    if (1 <= result.values.size()) {
        BOOST_TEST(it->pos == 0);
        BOOST_TEST(it->str == "a");
    }

    ++it;
    if (2 <= result.values.size()) {
        BOOST_TEST(it->pos == 1);
        BOOST_TEST(it->str == "b");
    }

    result = find_bf("Eventually it is last test!", "?s?");

    BOOST_TEST(result.values.size() == 3);

    it = result.values.begin();
    if (1 <= result.values.size()) {
        BOOST_TEST(it->pos == 14);
        BOOST_TEST(it->str == "is ");
    }

    ++it;
    if (2 <= result.values.size()) {
        BOOST_TEST(it->pos == 18);
        BOOST_TEST(it->str == "ast");
    }

    ++it;
    if (3 <= result.values.size()) {
        BOOST_TEST(it->pos == 23);
        BOOST_TEST(it->str == "est");
    }
}

BOOST_AUTO_TEST_CASE(split_test)
{
    constexpr auto hc = 8;
    auto [result, chars_count] = split("aaaabbbbccccddddXX", 3, hc);

    BOOST_TEST(chars_count == 2);
    BOOST_TEST(result.size() == 8);

    auto it = result.begin();
    if (1 <= result.size()) {
        BOOST_TEST(*it == "aaaa");
    }

    ++it;
    if (2 <= result.size()) {
        BOOST_TEST(*it == "aabb");
    }

    ++it;
    if (3 <= result.size()) {
        BOOST_TEST(*it == "bbbb");
    }

    ++it;
    if (4 <= result.size()) {
        BOOST_TEST(*it == "bbcc");
    }

    ++it;
    if (5 <= result.size()) {
        BOOST_TEST(*it == "cccc");
    }

    ++it;
    if (6 <= result.size()) {
        BOOST_TEST(*it == "ccdd");
    }

    ++it;
    if (7 <= result.size()) {
        BOOST_TEST(*it == "dddd");
    }

    ++it;
    if (8 <= result.size()) {
        BOOST_TEST(*it == "ddXX");
    }

    std::tie(result, chars_count) = split("abcdexx", 3, hc);

    BOOST_TEST(chars_count == 1);
    BOOST_TEST(result.size() == 5);

    it = result.begin();
    if (1 <= result.size()) {
        BOOST_TEST(*it == "abc");
    }

    ++it;
    if (2 <= result.size()) {
        BOOST_TEST(*it == "bcd");
    }

    ++it;
    if (3 <= result.size()) {
        BOOST_TEST(*it == "cde");
    }

    ++it;
    if (4 <= result.size()) {
        BOOST_TEST(*it == "dex");
    }

    ++it;
    if (5 <= result.size()) {
        BOOST_TEST(*it == "exx");
    }

    std::tie(result, chars_count) = split("XXXX'XXXX'XXXX'XXX123abcd", 3, hc);

    BOOST_TEST(chars_count == 3);
    BOOST_TEST(result.size() == 8);

    if (7 <= result.size())
    {
        auto it = result.rbegin();
        BOOST_TEST(*it == "abcd");

        ++it;
        BOOST_TEST(*it == "123ab");
    }

}

BOOST_AUTO_TEST_CASE(split_no_zero_devision)
{
    auto [result, chars_count] = split("x", 1, 3);

    BOOST_TEST(result.size() == 1);
    BOOST_TEST(result.front() == "x");
}

BOOST_AUTO_TEST_SUITE_END()
