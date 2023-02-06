#include "mtfind.h"

#include <cassert>
#include <vector>
#include <list>
#include <optional>
#include <future>
#include <ranges>

namespace detail {

Result parse(std::string_view in, std::string_view mask)
{
    assert(!mask.empty());

    Result result;
    auto& r = result.values;

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
            auto b = in.begin() + i;
            auto e = b + mask.size();
            r.emplace_back(i, std::string{b,e});

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

    const auto chars_count = (in.size() - mask_length + 1) / top_limit;

    if (chars_count == 0)
        return divide_string(in, mask_length, top_limit - 1);

    std::vector<std::string_view> result(top_limit);

    for (std::size_t i = 0; i < result.size(); ++i)
    {
        using namespace std::views;
        auto items = in | drop(i*chars_count) | take(chars_count + mask_length - 1);
        result[i] = { items.begin(), items.end() };
    }

    return { std::move(result), chars_count };
}

}

std::list<Match> mkfind(std::function<bool(std::string*)> stringReader, std::string_view mask)
{
    assert(0 < mask.size());
    using namespace detail;

    const auto hc = std::thread::hardware_concurrency();
    std::list<Match> results;
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

                for (auto& v: parsed.values) {
                    v.pos += j * chars_count;
                }

                parsed.line = line_index;
                return std::optional { std::move(parsed) };
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