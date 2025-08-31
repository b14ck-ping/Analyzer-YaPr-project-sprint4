#include "metric_impl/cyclomatic_complexity.hpp"

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

// здесь ваш код
MetricResult::ValueType CyclomaticComplexityMetric::CalculateImpl(const function::Function &f) const {
    MetricResult::ValueType line_counter = 0;

    std::string_view delim{"\n"};
    auto filtered_view = f.ast | rv::split(delim) /*  |
                     rv::transform(rv::drop_while([](const char c){return c == ' ' || c == '\t';})) */
        ;

    return 0;
}

std::string CyclomaticComplexityMetric::Name() const { return std::string("cyclomatic_complexity"); }

}  // namespace analyser::metric::metric_impl
