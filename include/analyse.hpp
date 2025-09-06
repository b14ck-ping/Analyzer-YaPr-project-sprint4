#include <iterator>
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
#include <iomanip>
#include <iostream>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

#include "file.hpp"
#include "function.hpp"
#include "metric.hpp"
#include "metric_accumulator.hpp"

namespace analyser {

namespace rv = std::ranges::views;
namespace rs = std::ranges;

inline auto AnalyseFunctions(const std::vector<std::string> &files,
                             const analyser::metric::MetricExtractor &metric_extractor)
    -> std::vector<std::pair<function::Function, analyser::metric::MetricResults>> {

    using out_item_type = std::pair<function::Function, analyser::metric::MetricResults>;

    auto file_creator = [](const std::string &file_name) { return analyser::file::File(file_name); };
    auto function_extraction = [](const analyser::file::File &file) {
        return analyser::function::FunctionExtractor{}.Get(file);
    };
    auto metric_extraction = [&metric_extractor](const function::Function &func) {
        return std::pair{func, metric_extractor.Get(func)};
    };

    auto functions_metrics_view = files | rv::transform(file_creator) | rv::transform(function_extraction) | rv::join |
                                  rv::transform(metric_extraction);

    return rs::to<std::vector<out_item_type>>(functions_metrics_view);
}

auto SplitByClasses(const auto &analysis) {}

auto SplitByFiles(const auto &analysis) {}

void AccumulateFunctionAnalysis(const auto &analysis,
                                const analyser::metric_accumulator::MetricsAccumulator &accumulator) {
    rs::for_each(analysis, )
}

}  // namespace analyser
