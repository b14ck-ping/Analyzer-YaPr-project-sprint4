#include "metric_impl/code_lines_count.hpp"

#include <gtest/gtest.h>
#include <string>
#include <tuple>

namespace analyser::metric::metric_impl {

class CodeLinesTest : public testing::TestWithParam<std::tuple<std::string, int>> {
protected:
    void SetUp() override {
        file_path = std::get<0>(GetParam());
        expected = std::get<1>(GetParam());
    }

    std::string file_path;
    double expected;
};

TEST_P(CodeLinesTest, from_files_tests) {
    CodeLinesCountMetric metric{};

    auto function = analyser::function::FunctionExtractor{}.Get(analyser::file::File(file_path));
    auto metric_res = metric.Calculate(function.back());

    EXPECT_EQ(metric_res.value, expected);
}

INSTANTIATE_TEST_SUITE_P(CodeLinesTestFromFile, CodeLinesTest,
                         testing::Values(std::make_tuple("../../../src/metric_impl/tests/files/comments.py", 4),
                                         std::make_tuple("../../../src/metric_impl/tests/files/exceptions.py", 8),
                                         std::make_tuple("../../../src/metric_impl/tests/files/if.py", 4),
                                         std::make_tuple("../../../src/metric_impl/tests/files/loops.py", 7),
                                         std::make_tuple("../../../src/metric_impl/tests/files/many_lines.py", 11),
                                         std::make_tuple("../../../src/metric_impl/tests/files/many_parameters.py", 2),
                                         std::make_tuple("../../../src/metric_impl/tests/files/match_case.py", 8),
                                         std::make_tuple("../../../src/metric_impl/tests/files/nested_if.py", 9),
                                         std::make_tuple("../../../src/metric_impl/tests/files/simple.py", 6),
                                         std::make_tuple("../../../src/metric_impl/tests/files/ternary.py", 2)));

}  // namespace analyser::metric::metric_impl
