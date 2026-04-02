#include <unity.h>

#include "SmaFilter.h"

void setUp(void) {
}

void tearDown(void) {
}

void test_function_should_calculate_correct_average(void) {
    SmaFilter filter(5);
    filter.addValue(1.0f);
    filter.addValue(2.0f);
    filter.addValue(3.0f);
    filter.addValue(4.0f);
    filter.addValue(5.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 3.0f, filter.getAverage());
}

void test_function_should_handle_more_values_than_window_size(void) {
    SmaFilter filter(3);
    filter.addValue(1.0f);
    filter.addValue(2.0f);
    filter.addValue(3.0f);
    // This value should overwrite the first value (1.0f)
    filter.addValue(4.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 3.0f, filter.getAverage());
}

void test_function_should_handle_less_values_than_window_size(void) {
    SmaFilter filter(5);
    filter.addValue(1.0f);
    filter.addValue(2.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 1.5f, filter.getAverage());
}

void test_function_should_handle_no_values(void) {
    SmaFilter filter(5);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, filter.getAverage());
}

void test_function_should_handle_window_size_exceeding_max_calculates_last_20_values(void) {
    SmaFilter filter(SmaFilter::MAX_WINDOW_SIZE + 5);
    for (int i = 0; i < 25; ++i) {
        filter.addValue(static_cast<int16_t>(i));
    }

    TEST_ASSERT_FLOAT_WITHIN(0.01f, 14.5f, filter.getAverage());
}

void test_function_should_handle_iterative_additions_and_calculate_average_correctly(void) {
    SmaFilter filter(3);
    filter.addValue(1.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 1.0f, filter.getAverage());

    filter.addValue(2.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 1.5f, filter.getAverage());

    filter.addValue(3.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 2.0f, filter.getAverage());

    filter.addValue(4.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 3.0f, filter.getAverage());
}

int runUnityTests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_function_should_calculate_correct_average);
    RUN_TEST(test_function_should_handle_more_values_than_window_size);
    RUN_TEST(test_function_should_handle_less_values_than_window_size);
    RUN_TEST(test_function_should_handle_no_values);
    RUN_TEST(test_function_should_handle_window_size_exceeding_max_calculates_last_20_values);
    RUN_TEST(test_function_should_handle_iterative_additions_and_calculate_average_correctly);
    return UNITY_END();
}

int main(void) {
    return runUnityTests();
}
