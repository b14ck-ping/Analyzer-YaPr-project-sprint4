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

using AnalyseResultItem = std::pair<function::Function, analyser::metric::MetricResults>;
using AnalyseResults = std::vector<AnalyseResultItem>;

inline auto AnalyseFunctions(const std::vector<std::string> &files,
                             const analyser::metric::MetricExtractor &metric_extractor) -> AnalyseResults {

    auto file_creator = [](const std::string &file_name) { return analyser::file::File(file_name); };
    auto function_extraction = [](const analyser::file::File &file) {
        return analyser::function::FunctionExtractor{}.Get(file);
    };
    auto metric_extraction = [&metric_extractor](const function::Function &func) {
        return std::pair{func, metric_extractor.Get(func)};
    };

    auto functions_metrics_view = files | rv::transform(file_creator) | rv::transform(function_extraction) | rv::join |
                                  rv::transform(metric_extraction);

    return rs::to<AnalyseResults>(functions_metrics_view);
}

auto SplitByClasses(const auto &analysis) -> std::unordered_map<std::string, AnalyseResults> {

    auto filter_by_classes = [](AnalyseResultItem &func) { return func.first.class_name.has_value(); };
    auto less_classname = [](const AnalyseResultItem &a, const AnalyseResultItem &b) {
        return *a.first.class_name < *b.first.class_name;
    };
    auto chunker = [](const AnalyseResultItem &a, const AnalyseResultItem &b) {
        return *a.first.class_name == *b.first.class_name;
    };
    auto transformer = [](const auto &chunk) {
        AnalyseResults vec;
        vec = rs::to<std::vector>(chunk);
        return std::make_pair(*vec.front().first.class_name, std::move(vec));
    };

    auto sorted_filtered_analisys =
        analysis | rs::to<std::vector>() | rv::filter(filter_by_classes) | rs::to<std::vector>();
    rs::sort(sorted_filtered_analisys, less_classname);

    return sorted_filtered_analisys | rs::to<std::vector>() | rv::chunk_by(chunker) | rv::transform(transformer) |
           rs::to<std::unordered_map<std::string, AnalyseResults>>();
}

inline auto SplitByFiles(const AnalyseResults &analysis) -> std::unordered_map<std::string, AnalyseResults> {

    auto less_filename = [](const AnalyseResultItem &a, const AnalyseResultItem &b) {
        return a.first.filename < b.first.filename;
    };
    auto chunker = [](const AnalyseResultItem &a, const AnalyseResultItem &b) {
        return a.first.filename == b.first.filename;
    };
    auto transformer = [](const auto &chunk) {
        AnalyseResults vec;
        vec = rs::to<std::vector>(chunk);
        return std::make_pair(vec.front().first.filename, std::move(vec));
    };

    auto sorted_filtered_analisys = rs::to<std::vector>(analysis);
    rs::sort(sorted_filtered_analisys, less_filename);

    return sorted_filtered_analisys | rs::to<std::vector>() | rv::chunk_by(chunker) | rv::transform(transformer) |
           rs::to<std::unordered_map<std::string, AnalyseResults>>();
}

void AccumulateFunctionAnalysis(const auto &analysis,
                                const analyser::metric_accumulator::MetricsAccumulator &accumulator) {
    rs::for_each(analysis, [&accumulator](const auto &item) {
        const auto &[func, metrics] = item;
        accumulator.AccumulateNextFunctionResults(metrics);
    });
}

}  // namespace analyser
