#include "mkfind.h"

#include <iostream>
#include <vector>
#include <thread>
#include <optional>

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

namespace {

auto parse(std::string_view in, std::string_view mask)
{
    std::vector<std::tuple<unsigned, std::string_view>> r;

    if (mask.empty()) return r;

    for (std::size_t i = 0, j = 0; i < in.size();)
    {
        for (j = 0; i < in.size(); ++j, ++i)
        {
            if (mask[j] != in[i] && mask[j] != '?') break;

            if (j + 1 == mask.size()) {
                auto pos = i - (mask.size() - 1);
                auto b = in.begin() + pos;
                auto e = b + mask.size();
                r.emplace_back(pos, std::string_view {b,e});
                break;
            }
        }
    }

    return r;
}

constexpr auto partice = 10ULL; // Будем считывать по partice строчек, парсить и записывать результат.

auto read_part_strings(std::ifstream &file)
{
    std::vector<std::string> lines(partice);
    std::string line;

    for (size_t i = 0; (i+1) % partice; ++i)
    {
        if (std::getline(file, line)) {
            lines[i % partice] = std::move(line);
        } else {
            lines.resize(i);
            return lines;
        }
    }

    return lines;
}

}

std::list<Result> mkfind(std::ifstream& file, std::string_view mask)
{
    const auto hc = std::thread::hardware_concurrency();
    boost::asio::thread_pool pool(hc == 0 ? 1 : hc);

    std::list<Result> results;

    for (std::size_t partice_number = 0;; ++partice_number)
    {
        const auto lines = read_part_strings(file);
        if (lines.empty()) break;

        std::vector<std::optional<Result>> part_results(lines.size());

        for (auto j = 0U; j < part_results.size(); ++j)
        {
            boost::asio::post(pool, [j, partice_number, mask, &part_results, &lines]{
                auto parsed = parse(lines[j], mask);

                if (parsed.empty())
                    return;

                part_results[j] = Result{};

                auto& v = part_results[j]->values;
                v.resize(parsed.size());

                for (size_t k = 0; k < parsed.size(); ++k) {
                    auto [pos, str] = parsed[k];
                    part_results[j]->line = j + partice_number * partice;
                    v[k].pos = pos;
                    v[k].str = str;
                }
            });
        }

        pool.wait();

        for (auto && result : part_results)
        {
            if (result.has_value()) {
                results.push_back(std::move(result.value()));
            }
        }
    }

    pool.join();
    return results;
}
