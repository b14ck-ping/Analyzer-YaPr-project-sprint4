#include "metric_accumulator_impl/sum_average_accumulator.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace analyser::metric_accumulator::metric_accumulator_impl::test {

constexpr double kEps = 1e-9;

inline analyser::metric::MetricResult MR(double v, std::string name = "test_metric") {
    analyser::metric::MetricResult m{};
    m.metric_name = std::move(name);
    m.value = v;
    return m;
}

TEST(SumAverageAccumulatorTest, EmptyAccFinalize) {
    SumAverageAccumulator acc;
    acc.Finalize();

    const auto &freq = acc.Get();
    ASSERT_EQ(freq.average, 0u);
    ASSERT_EQ(freq.sum, 0u);
}

TEST(SumAverageAccumulatorTest, SingleValueSumAndAverage) {
    SumAverageAccumulator acc;
    acc.Accumulate(MR(7));
    acc.Finalize();

    const auto r = acc.Get();
    EXPECT_EQ(r.sum, 7);
    EXPECT_NEAR(r.average, 7.0, kEps);
}

TEST(SumAverageAccumulatorTest, MultipleValuesSumAndAverage) {
    SumAverageAccumulator acc;
    acc.Accumulate(MR(5));
    acc.Accumulate(MR(15));
    acc.Accumulate(MR(20));
    acc.Finalize();

    const auto r = acc.Get();
    EXPECT_EQ(r.sum, 40);
    EXPECT_NEAR(r.average, 40.0 / 3.0, kEps);
}

TEST(SumAverageAccumulatorTest, HandlesZeroAndNegativeValues) {
    SumAverageAccumulator acc;
    acc.Accumulate(MR(-10));
    acc.Accumulate(MR(0));
    acc.Accumulate(MR(10));
    acc.Finalize();

    const auto r = acc.Get();
    EXPECT_EQ(r.sum, 0);
    EXPECT_NEAR(r.average, 0.0, kEps);
}

TEST(SumAverageAccumulatorTest, FinalizeIsIdempotent) {
    SumAverageAccumulator acc;
    for (int v : {2, 4, 6, 8})
        acc.Accumulate(MR(v));

    acc.Finalize();
    const auto first = acc.Get();

    acc.Finalize();
    const auto second = acc.Get();

    EXPECT_EQ(first.sum, 20);
    EXPECT_NEAR(first.average, 5.0, kEps);
    EXPECT_EQ(second.sum, first.sum);
    EXPECT_NEAR(second.average, first.average, kEps);
}

TEST(SumAverageAccumulatorTest, ResetClearsState) {
    SumAverageAccumulator acc;

    acc.Accumulate(MR(100));
    acc.Accumulate(MR(50));
    acc.Finalize();
    auto r1 = acc.Get();
    EXPECT_EQ(r1.sum, 150);
    EXPECT_NEAR(r1.average, 75.0, kEps);

    acc.Reset();
    acc.Accumulate(MR(1));
    acc.Accumulate(MR(2));
    acc.Accumulate(MR(3));
    acc.Finalize();
    auto r2 = acc.Get();
    EXPECT_EQ(r2.sum, 6);
    EXPECT_NEAR(r2.average, 2.0, kEps);
}

}  // namespace analyser::metric_accumulator::metric_accumulator_impl::test
