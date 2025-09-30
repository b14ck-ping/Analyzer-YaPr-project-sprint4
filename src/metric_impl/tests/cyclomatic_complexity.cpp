#include "metric_impl/cyclomatic_complexity.hpp"

#include <gtest/gtest.h>

namespace analyser::metric::metric_impl {

class CyclomaticComplexityTest : public testing::TestWithParam<std::tuple<std::string, int>> {
protected:
    void SetUp() override {
        file_path = std::get<0>(GetParam());
        expected = std::get<1>(GetParam());
    }

    std::string file_path;
    double expected;
};

TEST_P(CyclomaticComplexityTest, from_files_tests) {
    CyclomaticComplexityMetric metric{};

    auto function = analyser::function::FunctionExtractor{}.Get(analyser::file::File(file_path));
    auto metric_res = metric.Calculate(function.back());

    EXPECT_EQ(metric_res.value, expected);
}

INSTANTIATE_TEST_SUITE_P(CyclomaticComplexityTestFromFile, CyclomaticComplexityTest,
                         testing::Values(std::make_tuple("../../../src/metric_impl/tests/files/comments.py", 1),
                                         std::make_tuple("../../../src/metric_impl/tests/files/exceptions.py", 4),
                                         std::make_tuple("../../../src/metric_impl/tests/files/if.py", 2),
                                         std::make_tuple("../../../src/metric_impl/tests/files/loops.py", 4),
                                         std::make_tuple("../../../src/metric_impl/tests/files/many_lines.py", 2),
                                         std::make_tuple("../../../src/metric_impl/tests/files/many_parameters.py", 2),
                                         std::make_tuple("../../../src/metric_impl/tests/files/match_case.py", 4),
                                         std::make_tuple("../../../src/metric_impl/tests/files/nested_if.py", 5),
                                         std::make_tuple("../../../src/metric_impl/tests/files/simple.py", 2),
                                         std::make_tuple("../../../src/metric_impl/tests/files/ternary.py", 3)));

}  // namespace analyser::metric::metric_impl
