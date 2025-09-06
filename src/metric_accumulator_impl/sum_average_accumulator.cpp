#include "metric_accumulator_impl/sum_average_accumulator.hpp"

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
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace analyser::metric_accumulator::metric_accumulator_impl {

void SumAverageAccumulator::Accumulate(const metric::MetricResult &metric_result) {
    if (is_finalized)
        throw std::logic_error("Already finalized");

    this->sum += metric_result.value;
    ++this->count;
}

void SumAverageAccumulator::Finalize() {
    this->average = static_cast<double>(this->sum) / static_cast<double>(this->count);
    is_finalized = true;
}

void SumAverageAccumulator::Reset() {
    this->sum = 0;
    this->count = 0;
    this->average = 0;
    is_finalized = false;
}

SumAverageAccumulator::SumAverage SumAverageAccumulator::Get() const {
    if (!is_finalized)
        throw std::logic_error("Not finalized");

    return SumAverageAccumulator::SumAverage{sum, average};
}

}  // namespace analyser::metric_accumulator::metric_accumulator_impl
