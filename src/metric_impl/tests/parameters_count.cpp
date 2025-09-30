#include "metric_impl/parameters_count.hpp"

#include <gtest/gtest.h>

namespace analyser::metric::metric_impl {

TEST(CountParametersMetricTest, metric_name_n_value) {
    CountParametersMetric metric{};
    function::Function func{.ast = "(function_definition ...)"};
    auto metric_res = metric.Calculate(func);

    EXPECT_EQ(metric_res.metric_name, "parameters_count");
}

class CountParametersTest : public testing::TestWithParam<std::tuple<std::string, int>> {
protected:
    void SetUp() override {
        file_path = std::get<0>(GetParam());
        expected = std::get<1>(GetParam());
    }

    std::string file_path;
    double expected;
};

TEST_P(CountParametersTest, from_files_tests) {
    CountParametersMetric metric{};

    auto function = analyser::function::FunctionExtractor{}.Get(analyser::file::File(file_path));
    auto metric_res = metric.Calculate(function.back());

    EXPECT_EQ(metric_res.value, expected);
}

INSTANTIATE_TEST_SUITE_P(CountParametersMetricTest, CountParametersTest,
                         testing::Values(std::make_tuple("../../../src/metric_impl/tests/files/comments.py", 3),
                                         std::make_tuple("../../../src/metric_impl/tests/files/exceptions.py", 0),
                                         std::make_tuple("../../../src/metric_impl/tests/files/if.py", 1),
                                         std::make_tuple("../../../src/metric_impl/tests/files/loops.py", 1),
                                         std::make_tuple("../../../src/metric_impl/tests/files/many_lines.py", 0),
                                         std::make_tuple("../../../src/metric_impl/tests/files/many_parameters.py", 5),
                                         std::make_tuple("../../../src/metric_impl/tests/files/match_case.py", 1),
                                         std::make_tuple("../../../src/metric_impl/tests/files/nested_if.py", 2),
                                         std::make_tuple("../../../src/metric_impl/tests/files/simple.py", 0),
                                         std::make_tuple("../../../src/metric_impl/tests/files/ternary.py", 1)));

}  // namespace analyser::metric::metric_impl
