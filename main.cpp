#include <exception>
#include <memory>
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

#include "analyse.hpp"
#include "cmd_options.hpp"
#include "file.hpp"
#include "function.hpp"
#include "metric.hpp"
#include "metric_accumulator.hpp"
#include "metric_accumulator_impl/accumulators.hpp"
#include "metric_impl/metrics.hpp"


namespace metric_impl = analyser::metric::metric_impl;

int main(int argc, char *argv[]) {
    analyser::cmd::ProgramOptions options;
    // распарсите входные параметры
    if (!options.Parse(argc, argv))
        return -1;

    std::vector<std::string> files = options.GetFiles();

    if (files.size() == 0)
        return -2;

    // регистрируем метрики 
    analyser::metric::MetricExtractor metric_extractor{};
    metric_extractor.RegisterMetric(std::make_unique<metric_impl::CodeLinesCountMetric>());
    metric_extractor.RegisterMetric(std::make_unique<metric_impl::CyclomaticComplexityMetric>());
    metric_extractor.RegisterMetric(std::make_unique<metric_impl::CountParametersMetric>());

    try {
        // запустите analyser::AnalyseFunctions
        auto functions = analyser::AnalyseFunctions(files, metric_extractor);

        // выведете результаты анализа на консоль
        rs::for_each (functions, [](const auto &res) {
            auto [func, metrics] = res;
            std::println("{}{}::{}:", func.filename, func.class_name.has_value() ? std::string("::") + *func.class_name : "", func.name);
            rs::for_each (metrics, [](const auto &metric) {
                std::println("\t{}: {}", metric.metric_name, metric.value);
            });
        });
        std::println();

        using namespace analyser::metric_accumulator;
        MetricsAccumulator accumulator;
        // зарегистрируйте аккумуляторы метрик в accumulator
        accumulator.RegisterAccumulator("code_lines_count", std::make_unique<metric_accumulator_impl::SumAverageAccumulator>());
        accumulator.RegisterAccumulator("cyclomatic_complexity", std::make_unique<metric_accumulator_impl::SumAverageAccumulator>());
        accumulator.RegisterAccumulator("parameters_count", std::make_unique<metric_accumulator_impl::AverageAccumulator>());

        // запустите analyser::SplitByFiles
        auto by_files = analyser::SplitByFiles(functions);

        // запустите analyser::AccumulateFunctionAnalysis для каждого подмножества результатов метрик
        rs::for_each (by_files, [&accumulator](auto& file) {
            auto [file_name, analysis] = file;
            std::println("Accumulated Analysis for file {}:", file_name);
            accumulator.ResetAccumulators();
        

            analyser::AccumulateFunctionAnalysis(analysis, accumulator);

            auto lines = accumulator.GetFinalizedAccumulator<metric_accumulator_impl::SumAverageAccumulator>("code_lines_count").Get();
            auto complexity = accumulator.GetFinalizedAccumulator<metric_accumulator_impl::SumAverageAccumulator>("cyclomatic_complexity").Get();
            auto params = accumulator.GetFinalizedAccumulator<metric_accumulator_impl::AverageAccumulator>("parameters_count").Get();

            std::println("\tcode_lines_count: {}:", lines.sum);
            std::println("\tcyclomatic_complexity: {}:", complexity.sum);
            std::println("\tparameters_count: {}:", params);
        });
        std::println();

        // запустите analyser::SplitByClasses
        auto by_classes = analyser::SplitByClasses(functions);

        // запустите analyser::AccumulateFunctionAnalysis для каждого подмножества результатов метрик
        rs::for_each (by_classes, [&accumulator](auto& file) {
            auto [file_name, analysis] = file;
            std::println("Accumulated Analysis for class {}:", file_name);
            accumulator.ResetAccumulators();
        

            analyser::AccumulateFunctionAnalysis(analysis, accumulator);

            auto lines = accumulator.GetFinalizedAccumulator<metric_accumulator_impl::SumAverageAccumulator>("code_lines_count").Get();
            auto complexity = accumulator.GetFinalizedAccumulator<metric_accumulator_impl::SumAverageAccumulator>("cyclomatic_complexity").Get();
            auto params = accumulator.GetFinalizedAccumulator<metric_accumulator_impl::AverageAccumulator>("parameters_count").Get();

            std::println("\tcode_lines_count: {}:", lines.sum);
            std::println("\tcyclomatic_complexity: {}:", complexity.sum);
            std::println("\tparameters_count: {}:", params);
        });
        std::println();


        // запустите analyser::AccumulateFunctionAnalysis для всех результатов метрик
        std::println("Accumulated Analysis for all results:");
        accumulator.ResetAccumulators();

        analyser::AccumulateFunctionAnalysis(functions, accumulator);

        auto lines = accumulator.GetFinalizedAccumulator<metric_accumulator_impl::SumAverageAccumulator>("code_lines_count").Get();
        auto complexity = accumulator.GetFinalizedAccumulator<metric_accumulator_impl::SumAverageAccumulator>("cyclomatic_complexity").Get();

        std::println("\tcode_lines_count: {}:", lines.sum);
        std::println("\tcyclomatic_complexity: {}:", complexity.sum);
    } catch (const std::exception &e) {
        std::print(std::cerr, "Error: {}\n", e.what());
        return 1;
    }

    return 0;
}
