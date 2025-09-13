#include "metric_accumulator_impl/average_accumulator.hpp"

#include <stdexcept>
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

void AverageAccumulator::Accumulate(const analyser::metric::MetricResult &metric_result) {
    if (is_finalized)
        throw std::logic_error("Already finalized");

    this->sum += metric_result.value;
    ++this->count;
}

void AverageAccumulator::Finalize() {
    if (this->count != 0)
        this->average = static_cast<double>(this->sum) / static_cast<double>(this->count);
    is_finalized = true;
}

void AverageAccumulator::Reset() {
    this->sum = 0;
    this->count = 0;
    this->average = 0;
    is_finalized = false;
}

double AverageAccumulator::Get() const {
    if (!is_finalized)
        throw std::logic_error("Not finalized");

    return this->average;
}

}  // namespace analyser::metric_accumulator::metric_accumulator_impl
