#include "metric_impl/parameters_count.hpp"

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace analyser::metric::metric_impl {

MetricResult::ValueType CountParametersMetric::CalculateImpl(const function::Function &f) const {
    MetricResult::ValueType line_counter = 0;

    std::string_view delim{"\n"};
    auto filtered_view =
        f.ast | rv::split(delim) |
        rv::transform([](auto &&subrange) { return std::string_view(subrange.begin(), subrange.end()); }) |
        rv::drop_while([](std::string_view line) { return line.find("parameters:") == std::string_view::npos; });

    auto parameters_block_range = rs::subrange(
        rs::next(rs::begin(filtered_view)), rs::find_if(filtered_view, [](std::string_view line) {
            return line.find("body:") != std::string_view::npos || line.find("return_type:") != std::string_view::npos;
        }));

    size_t main_offset =
        rs::distance((*parameters_block_range.begin()).begin(),
                     rs::find_if(*parameters_block_range.begin(), [](const char c) { return c != ' '; }));

    auto params_list_view = rv::all(parameters_block_range) | rv::transform([main_offset](std::string_view sv) {
                                size_t offset = std::min(main_offset, sv.size());
                                return std::string_view{sv.begin() + offset, sv.end()};
                            }) |
                            rv::filter([](std::string_view sv) { return *sv.begin() != ' '; });

    return rs::distance(params_list_view.begin(), params_list_view.end());
}

std::string CountParametersMetric::Name() const { return std::string("parameters_count"); }

}  // namespace analyser::metric::metric_impl
