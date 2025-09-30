#include "metric_impl/code_lines_count.hpp"

#include <charconv>
#include <cstddef>
#include <print>
#include <string_view>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <ranges>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_set>
#include <variant>
#include <vector>

namespace analyser::metric::metric_impl {

MetricResult::ValueType CodeLinesCountMetric::CalculateImpl(const function::Function &f) const {
    std::unordered_set<size_t> lines;

    std::string_view delim{"\n"};
    auto filtered_view =
        f.ast | rv::split(delim) |
        rv::transform([](auto &&substr) { return std::string_view(&*substr.begin(), std::ranges::distance(substr)); }) |
        rv::filter([](std::string_view line) { return line.find("comment") == std::string_view::npos; }) |
        rs::to<std::vector>();

    std::regex word_regex{R"(\[(\d+),\s*\d+\])"};
    std::ranges::for_each(filtered_view, [&word_regex, &lines](std::string_view line) {
        std::string line_str(line);
        auto i = std::sregex_iterator(line_str.begin(), line_str.end(), word_regex);
        if (i != std::sregex_iterator()) {
            size_t line_number = 0;
            std::string match_str = (i->begin() + 1)->str();
            std::from_chars(match_str.c_str(), match_str.c_str() + match_str.size(), line_number);
            lines.insert(line_number);
        }
    });
    return lines.size();
}

std::string CodeLinesCountMetric::Name() const { return std::string("code_lines_count"); }

}  // namespace analyser::metric::metric_impl
