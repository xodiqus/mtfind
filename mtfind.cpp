#include "mtfind.h"

#include <cassert>
#include <vector>
#include <optional>
#include <future>
#include <ranges>
#include <deque>

namespace mtfind {

Result parse(std::string_view in, std::string_view mask)
{
    assert(!mask.empty());

    Result result;
    auto& values = result.values;

    // Compares from the first char for mask.size().
    constexpr auto eq = [](std::string_view in, std::string_view mask)
    {
        if (in.size() < mask.size()) {
            return false;
        }

        for (std::size_t i = 0; i < in.size() && i < mask.size(); ++i)
        {
            if (mask[i] != in[i] && mask[i] != '?') {
                return false;
            }
        }

        return true;
    };

    for (std::size_t i = 0; i < in.size(); ++i)
    {
        if (eq({in.begin() + i, in.end()}, mask))
        {
            auto begin = in.begin() + i;
            auto end   = begin + mask.size();
            values.emplace_back(i, std::string{begin,end});

            i += mask.size() - 1;
        }
    }

    return result;
}

std::tuple<std::vector<std::string_view>, std::size_t> divide_string(std::string_view in, std::size_t mask_length, std::size_t top_limit)
{
    assert(!in.empty());
    assert(0 < mask_length);
    assert(0 < top_limit);
    assert(mask_length - 1 < in.size());

    // There is no devision by zero:
    // 1) A user can't put top_limit = 0 because of the assert above.
    // 2) The recursion below can't come the zero devision:
    //		For all top_limit != 0 and all in.size() != 0, the recurent calls have any top_limit != 0.
    // 		Example:
    // 				divide_string("x", _, 3); // chars_count = 1 / 3 = 0;
    //				divide_string("x", _, 2); // chars_count = 1 / 2 = 0;
    // 				divide_string("x", _, 1); // chars_count = 1 / 1 = 1; The recursion finish.
    const auto chars_count = in.size() / top_limit;

    if (chars_count == 0)
        return divide_string(in, mask_length, top_limit - 1);

    std::vector<std::string_view> result(top_limit);

    std::size_t i = 0;
    for (; i < result.size(); ++i)
    {
        using namespace std::views;

        auto temp = in | drop(i*chars_count) | take(chars_count + mask_length - 1);
        auto item = std::string_view{ temp.begin(), temp.end() };

        if (item.size() < mask_length)
            break;

        result[i] = item;
    }

    result.resize(i);

    return { std::move(result), chars_count };
}

std::deque<Match> mkfind(std::function<bool(std::string*)> stringReader, std::string_view mask)
{
    assert(0 < mask.size());

    const auto hc = std::thread::hardware_concurrency();
    std::deque<Match> results;
    std::string line;

    for (auto line_index = 0U; stringReader(&line); ++line_index)
    {
        const auto [parts, chars_count] = divide_string(line, mask.size(), hc == 0 ? 1 : hc);
        std::vector<std::future<std::optional<Result>>> part_results(parts.size());

        for (auto j = 0U; j < parts.size(); ++j)
        {
            part_results[j] = std::async(std::launch::async, [=, &parts]
            {
                auto parsed = parse(parts[j], mask);

                if (parsed.values.empty()) {
                    return std::optional<Result>{};
                }

                for (auto& value: parsed.values) {
                    value.pos += j * chars_count;
                }

                parsed.line = line_index;
                return std::optional { std::move(parsed) };
            });
        }

        for (auto && result: part_results)
        {
            auto optional = result.get();
            if (optional.has_value()) {
                for (auto && [pos, str]: optional->values)
                {
                    results.emplace_back(optional->line, pos, std::move(str));
                }
            }
        }
    }

    return results;
}

}

