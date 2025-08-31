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


    // запустите analyser::AnalyseFunctions
    auto functions = analyser::AnalyseFunctions(files, metric_extractor);
    // выведете результаты анализа на консоль
    for (auto func : functions){
        std::println("{}{}::{}:", func.first.filename, func.first.class_name ? std::string("::") + *func.first.class_name : "", func.first.name);
        for (auto metric : func.second)
            std::println("\t{}: {}", metric.metric_name, metric.value);
    }

    // analyser::metric_accumulator::MetricsAccumulator accumulator;
    // зарегистрируйте аккумуляторы метрик в accumulator

    // запустите analyser::SplitByFiles
    // запустите analyser::AccumulateFunctionAnalysis для каждого подмножества результатов метрик
    
    // выведете результаты на консоль

    // запустите analyser::SplitByClasses
    // запустите analyser::AccumulateFunctionAnalysis для каждого подмножества результатов метрик
    // выведете результаты на консоль

    // запустите analyser::AccumulateFunctionAnalysis для всех результатов метрик
    // выведете результаты на консоль

    return 0;
}
