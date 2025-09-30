#include "metric_accumulator_impl/average_accumulator.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace analyser::metric_accumulator::metric_accumulator_impl::test {

constexpr double kEps = 1e-9;

inline analyser::metric::MetricResult MR(int v, std::string name = "test_metric") {
    analyser::metric::MetricResult m{};
    m.metric_name = std::move(name);
    m.value = v;
    return m;
}

TEST(AverageAccumulatorTest, EmptyAccFinalize) {
    AverageAccumulator acc;
    acc.Finalize();

    const auto &freq = acc.Get();
    ASSERT_EQ(freq, 0u);
}

TEST(AverageAccumulatorTest, SingleValueEqualsItself) {
    AverageAccumulator acc;
    acc.Accumulate(MR(42));
    acc.Finalize();

    EXPECT_NEAR(acc.Get(), 42.0, kEps);
}

TEST(AverageAccumulatorTest, MultipleValuesComputeCorrectAverage) {
    AverageAccumulator acc;
    acc.Accumulate(MR(10));
    acc.Accumulate(MR(20));
    acc.Accumulate(MR(30));
    acc.Finalize();

    EXPECT_NEAR(acc.Get(), (10.0 + 20.0 + 30.0) / 3.0, kEps);
}

TEST(AverageAccumulatorTest, HandlesZeroAndNegativeValues) {
    AverageAccumulator acc;
    acc.Accumulate(MR(0));
    acc.Accumulate(MR(-5));
    acc.Accumulate(MR(5));
    acc.Accumulate(MR(-10));
    acc.Accumulate(MR(10));
    acc.Finalize();

    EXPECT_NEAR(acc.Get(), 0.0, kEps);
}

TEST(AverageAccumulatorTest, FinalizeIsIdempotent) {
    AverageAccumulator acc;
    acc.Accumulate(MR(1));
    acc.Accumulate(MR(2));
    acc.Accumulate(MR(3));
    acc.Finalize();
    const double first = acc.Get();

    acc.Finalize();
    const double second = acc.Get();

    EXPECT_NEAR(first, 2.0, kEps);
    EXPECT_NEAR(second, first, kEps);
}

TEST(AverageAccumulatorTest, ResetClearsState) {
    AverageAccumulator acc;

    acc.Accumulate(MR(100));
    acc.Accumulate(MR(0));
    acc.Finalize();
    EXPECT_NEAR(acc.Get(), 50.0, kEps);

    acc.Reset();
    acc.Accumulate(MR(3));
    acc.Accumulate(MR(3));
    acc.Accumulate(MR(3));
    acc.Finalize();
    EXPECT_NEAR(acc.Get(), 3.0, kEps);
}

}  // namespace analyser::metric_accumulator::metric_accumulator_impl::test
