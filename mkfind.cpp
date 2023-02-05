#include "mkfind.h"

#include <cassert>
#include <vector>
#include <list>
#include <optional>
#include <future>

namespace {

struct Result
{
    std::size_t line;

    struct Data {
        std::size_t pos;
        std::string str;
    };

    std::list<Data> values;
};

auto parse(std::string_view in, std::string_view mask)
{
    assert(!mask.empty());

    Result result;
    auto& r = result.values;

    for (std::size_t i = 0, j = 0; i < in.size();)
    {
        for (j = 0; i < in.size(); ++j, ++i)
        {
            if (mask[j] != in[i] && mask[j] != '?') break;

            if (j + 1 == mask.size()) {
                auto pos = i - (mask.size() - 1);
                auto b = in.begin() + pos;
                auto e = b + mask.size();
                r.emplace_back(pos, std::string{b,e});
                break;
            }
        }
    }

    return result;
}

auto divide_string(std::string_view in, std::string_view mask, std::size_t chars_count)
{
    assert(!mask.empty());
    assert(chars_count != 0);

    const auto div = in.size() / chars_count;
    const auto rem = in.size() % chars_count;

    std::vector<std::string_view> result(div + (mask.size() < rem));

    auto b = in.begin();
    for (std::size_t i = 0; i < result.size() - (rem != 0); ++i)
    {
        result[i] = { b, b + chars_count + mask.size() };
        b += chars_count;
    }

    if (rem != 0) {
        result[result.size() - 1] = { b, in.end() };
    }

    return result;
}

}

std::list<Match> mkfind(std::ifstream& file, std::string_view mask)
{
    const auto hc = std::thread::hardware_concurrency();

    std::list<Match> results;
    std::string line;

    for (auto line_index = 0U; std::getline(file, line); ++line_index)
    {
        assert(1 < mask.size());
        const std::size_t chars_count = line.size() / (mask.size() - 1);

        const auto parts = divide_string(line, mask, chars_count);
        std::vector<std::future<std::optional<Result>>> part_results(parts.size());

        for (auto j = 0U; j < parts.size(); ++j)
        {
            part_results[j] = std::async(std::launch::async, [=, &parts]
            {
                auto parsed = parse(parts[j], mask);

                if (parsed.values.empty()) {
                    return std::optional<Result>{};
                }

                for (auto& v: parsed.values) {
                    v.pos += j * chars_count;
                }

                parsed.line = line_index;
                return std::optional{parsed};

            });
        }

        for (auto && result: part_results)
        {
            auto opt = result.get();
            if (opt.has_value()) {
                for (auto && [pos, str]: opt->values)
                {
                    results.emplace_back(opt->line, pos, std::move(str));
                }
            }
        }
    }

    return results;
}
