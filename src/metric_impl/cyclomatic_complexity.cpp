#include "metric_impl/cyclomatic_complexity.hpp"

#include <string_view>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace analyser::metric::metric_impl {

const std::vector<std::string_view> expressions = {"if_statement",           "elif_clause",   "while_statement",
                                                   "try_statement",          "except_clause", "for_statement",
                                                   "conditional_expression", "case_clause",   "assert_statement"};

MetricResult::ValueType CyclomaticComplexityMetric::CalculateImpl(const function::Function &f) const {
    MetricResult::ValueType cyclomatic_complexity = 0;

    std::string_view delim{"\n"};
    auto body_block_range =
        f.ast | rv::split(delim) |
        rv::transform([](auto &&subrange) { return std::string_view(subrange.begin(), subrange.end()); }) |
        rv::drop_while([](std::string_view line) { return line.find("body:") == std::string_view::npos; });

    long complexity = rs::count_if(body_block_range, [](std::string_view sv) {
        auto item = rs::find_if(expressions,
                                [sv](const std::string_view expr) { return sv.find(expr) != std::string_view::npos; });

        if (item != expressions.end()) {
            return true;
        } else
            return false;
    });

    ++complexity;

    return complexity;
}

std::string CyclomaticComplexityMetric::Name() const { return std::string("cyclomatic_complexity"); }

}  // namespace analyser::metric::metric_impl
