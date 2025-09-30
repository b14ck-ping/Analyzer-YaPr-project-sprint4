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

size_t offset_counter(std::string_view sv) {
    return rs::distance(sv.begin(), rs::find_if(sv, [](const char c) { return c != ' '; }));
};

MetricResult::ValueType CountParametersMetric::CalculateImpl(const function::Function &f) const {

    size_t main_offset = 0;

    auto lines_taker = [&main_offset](std::string_view sv) { return offset_counter(sv) != main_offset; };
    auto parameter_lines_filter = [&main_offset](std::string_view sv) { return offset_counter(sv) == main_offset + 2; };

    std::string_view delim{"\n"};
    auto filtered_view =
        f.ast | rv::split(delim) |
        rv::transform([](auto &&subrange) { return std::string_view(subrange.begin(), subrange.end()); }) |
        rv::drop_while([](std::string_view line) { return line.find("parameters:") == std::string_view::npos; });

    if (filtered_view.begin() == filtered_view.end())
        return 0;

    main_offset = offset_counter(*filtered_view.begin());

    auto params_list_view =
        filtered_view | rv::drop(1) | rv::take_while(lines_taker) | rv::filter(parameter_lines_filter);

    return rs::distance(params_list_view.begin(), params_list_view.end());
}

std::string CountParametersMetric::Name() const { return std::string("parameters_count"); }

}  // namespace analyser::metric::metric_impl
