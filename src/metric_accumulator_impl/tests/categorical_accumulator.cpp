

#include "metric_accumulator_impl/categorical_accumulator.hpp"

#include <gtest/gtest.h>

#include <stdexcept>

namespace analyser::metric_accumulator::metric_accumulator_impl::test {

constexpr double kEps = 1e-9;

inline analyser::metric::MetricResult MR_cat(std::string cat) {
    analyser::metric::MetricResult m{};
    m.metric_name = std::move(cat);
    return m;
}

TEST(CategoricalAccumulatorTest, EmptyAccFinalize) {
    CategoricalAccumulator acc;
    acc.Finalize();

    const auto &freq = acc.Get();
    ASSERT_EQ(freq.size(), 0u);
}

TEST(CategoricalAccumulatorTest, SingleCategoryCountsOnce) {
    CategoricalAccumulator acc;
    acc.Accumulate(MR_cat("Bug"));
    acc.Finalize();

    const auto &freq = acc.Get();
    ASSERT_EQ(freq.size(), 1u);
    auto it = freq.find("Bug");
    ASSERT_NE(it, freq.end());
    EXPECT_EQ(it->second, 1);
}

TEST(CategoricalAccumulatorTest, MultipleCategoriesWithFrequencies) {
    CategoricalAccumulator acc;
    acc.Accumulate(MR_cat("Bug"));
    acc.Accumulate(MR_cat("Bug"));
    acc.Accumulate(MR_cat("Feature"));
    acc.Accumulate(MR_cat("Refactor"));
    acc.Accumulate(MR_cat("Bug"));
    acc.Accumulate(MR_cat("Feature"));

    acc.Finalize();

    const auto &f = acc.Get();
    ASSERT_EQ(f.size(), 3u);
    EXPECT_EQ(f.at("Bug"), 3);
    EXPECT_EQ(f.at("Feature"), 2);
    EXPECT_EQ(f.at("Refactor"), 1);
}

TEST(CategoricalAccumulatorTest, CaseSensitivityDistinctKeys) {
    CategoricalAccumulator acc;
    acc.Accumulate(MR_cat("bug"));
    acc.Accumulate(MR_cat("Bug"));
    acc.Accumulate(MR_cat("BUG"));
    acc.Finalize();

    const auto &f = acc.Get();
    ASSERT_EQ(f.size(), 3u);
    EXPECT_EQ(f.at("bug"), 1);
    EXPECT_EQ(f.at("Bug"), 1);
    EXPECT_EQ(f.at("BUG"), 1);
}

TEST(CategoricalAccumulatorTest, FinalizeIsIdempotent) {
    CategoricalAccumulator acc;
    for (auto &c : {"A", "B", "A", "C", "B", "A"})
        acc.Accumulate(MR_cat(c));

    acc.Finalize();
    const auto first = acc.Get();

    acc.Finalize();
    const auto second = acc.Get();

    EXPECT_EQ(first, second);
    EXPECT_EQ(first.at("A"), 3);
    EXPECT_EQ(first.at("B"), 2);
    EXPECT_EQ(first.at("C"), 1);
}

TEST(CategoricalAccumulatorTest, ResetClearsState) {
    CategoricalAccumulator acc;

    acc.Accumulate(MR_cat("X"));
    acc.Accumulate(MR_cat("Y"));
    acc.Accumulate(MR_cat("X"));
    acc.Finalize();
    {
        const auto &f = acc.Get();
        ASSERT_EQ(f.size(), 2u);
        EXPECT_EQ(f.at("X"), 2);
        EXPECT_EQ(f.at("Y"), 1);
    }

    acc.Reset();
    acc.Accumulate(MR_cat("Z"));
    acc.Accumulate(MR_cat("Z"));
    acc.Finalize();
    {
        const auto &f2 = acc.Get();
        ASSERT_EQ(f2.size(), 1u);
        EXPECT_EQ(f2.at("Z"), 2);
        EXPECT_TRUE(f2.find("X") == f2.end());
        EXPECT_TRUE(f2.find("Y") == f2.end());
    }
}

}  // namespace analyser::metric_accumulator::metric_accumulator_impl::test
