#include <unity.h>

#include "Encoder.h"

class MockQuadraturePcnt : public IQuadraturePcnt {
  public:
    esp_err_t init(int16_t maxValue, int16_t minValue, uint16_t debounceNs) override {
        return ESP_OK;
    }

    esp_err_t tick(int16_t* position, Signal* clkState, Signal* dtState) override {
        *position = _position;
        *clkState = IQuadraturePcnt::HIGH;
        *dtState = IQuadraturePcnt::HIGH;
        return ESP_OK;
    }

    esp_err_t reset() override {
        _position = 0;
        return ESP_OK;
    }

    void setPosition(int16_t position) {
        _position = position;
    }

  private:
    int16_t _position{};
};

struct EncoderSut {
    MockQuadraturePcnt* mock;
    Encoder2 encoder;

    EncoderSut() : mock(new MockQuadraturePcnt()), encoder(std::unique_ptr<IQuadraturePcnt>(mock)) {
        encoder.init(20, -20, 1000);
        mock->setPosition(0);
    }
};

void setUp(void) {
}

void tearDown(void) {
}

void assertState(Encoder2& encoder,
                 int16_t expectedRaw,
                 Encoder2::Direction expectedDir,
                 int16_t expectedDetent,
                 int8_t expectedDelta,
                 bool expectedDirectionChanged) {
    Encoder2::State state;
    encoder.tick(&state);
    TEST_ASSERT_EQUAL(expectedRaw, state.rawPosition);
    TEST_ASSERT_EQUAL(expectedDir, state.direction);
    TEST_ASSERT_EQUAL(expectedDetent, state.detentPosition);
    TEST_ASSERT_EQUAL(expectedDelta, state.accumulatedDelta);
    TEST_ASSERT_EQUAL(expectedDirectionChanged, state.directionChanged);
}

struct TestStep {
    int16_t position;
    Encoder2::Direction expectedDir;
    int16_t expectedDetent;
    int8_t expectedDelta;
    bool expectedDirectionChanged;
};

void runSequenceTest(const TestStep* steps, size_t numSteps) {
    EncoderSut sut;
    for (size_t i = 0; i < numSteps; ++i) {
        sut.mock->setPosition(steps[i].position);
        assertState(sut.encoder,
                    steps[i].position,
                    steps[i].expectedDir,
                    steps[i].expectedDetent,
                    steps[i].expectedDelta,
                    steps[i].expectedDirectionChanged);
    }
}

void test_initial_state(void) {
    EncoderSut sut;
    assertState(sut.encoder, 0, Encoder2::Direction::NONE, 0, 0, false);
}

void test_from_initial_state_to_cw_second_detent(void) {
    const TestStep cwSteps[] = {{0, Encoder2::Direction::NONE, 0, 0, false},
                                {1, Encoder2::Direction::NONE, 0, 0, false},
                                {2, Encoder2::Direction::NONE, 0, 0, false},
                                {3, Encoder2::Direction::NONE, 0, 0, false},
                                {4, Encoder2::Direction::CW, 1, 1, false},
                                {5, Encoder2::Direction::CW, 1, 1, false},
                                {6, Encoder2::Direction::CW, 1, 1, false},
                                {7, Encoder2::Direction::CW, 1, 1, false},
                                {8, Encoder2::Direction::CW, 2, 2, false}};
    runSequenceTest(cwSteps, sizeof(cwSteps) / sizeof(TestStep));
}

void test_from_initial_state_to_ccw_second_detent(void) {
    const TestStep ccwSteps[] = {{0, Encoder2::Direction::NONE, 0, 0, false},
                                 {-1, Encoder2::Direction::NONE, 0, 0, false},
                                 {-2, Encoder2::Direction::NONE, 0, 0, false},
                                 {-3, Encoder2::Direction::NONE, 0, 0, false},
                                 {-4, Encoder2::Direction::CCW, -1, -1, false},
                                 {-5, Encoder2::Direction::CCW, -1, -1, false},
                                 {-6, Encoder2::Direction::CCW, -1, -1, false},
                                 {-7, Encoder2::Direction::CCW, -1, -1, false},
                                 {-8, Encoder2::Direction::CCW, -2, -2, false}};
    runSequenceTest(ccwSteps, sizeof(ccwSteps) / sizeof(TestStep));
}

void test_fast_cw(void) {
    const TestStep cwSteps[] = {{0, Encoder2::Direction::NONE, 0, 0, false},
                                {1, Encoder2::Direction::NONE, 0, 0, false},
                                {4, Encoder2::Direction::CW, 1, 1, false},
                                {8, Encoder2::Direction::CW, 2, 2, false},
                                {14, Encoder2::Direction::CW, 3, 3, false},
                                {15, Encoder2::Direction::CW, 3, 3, false},
                                {16, Encoder2::Direction::CW, 4, 4, false}};
    runSequenceTest(cwSteps, sizeof(cwSteps) / sizeof(TestStep));
}

void test_fast_ccw(void) {
    const TestStep ccwSteps[] = {{0, Encoder2::Direction::NONE, 0, 0, false},
                                 {-1, Encoder2::Direction::NONE, 0, 0, false},
                                 {-4, Encoder2::Direction::CCW, -1, -1, false},
                                 {-8, Encoder2::Direction::CCW, -2, -2, false},
                                 {-14, Encoder2::Direction::CCW, -3, -3, false},
                                 {-15, Encoder2::Direction::CCW, -3, -3, false},
                                 {-16, Encoder2::Direction::CCW, -4, -4, false}};
    runSequenceTest(ccwSteps, sizeof(ccwSteps) / sizeof(TestStep));
}

void test_cw_bounce(void) {
    const TestStep steps[] = {{0, Encoder2::Direction::NONE, 0, 0, false},
                              {4, Encoder2::Direction::CW, 1, 1, false},
                              {5, Encoder2::Direction::CW, 1, 1, false},
                              {7, Encoder2::Direction::CW, 1, 1, false},
                              {5, Encoder2::Direction::CW, 1, 1, false},
                              {8, Encoder2::Direction::CW, 2, 2, false}};
    runSequenceTest(steps, sizeof(steps) / sizeof(TestStep));
}

void test_ccw_bounce(void) {
    const TestStep steps[] = {{0, Encoder2::Direction::NONE, 0, 0, false},
                              {-4, Encoder2::Direction::CCW, -1, -1, false},
                              {-5, Encoder2::Direction::CCW, -1, -1, false},
                              {-7, Encoder2::Direction::CCW, -1, -1, false},
                              {-5, Encoder2::Direction::CCW, -1, -1, false},
                              {-8, Encoder2::Direction::CCW, -2, -2, false}};
    runSequenceTest(steps, sizeof(steps) / sizeof(TestStep));
}

void test_direction_change_cw_to_ccw(void) {
    const TestStep steps[] = {{0, Encoder2::Direction::NONE, 0, 0, false},
                              {4, Encoder2::Direction::CW, 1, 1, false},
                              {8, Encoder2::Direction::CW, 2, 2, false},
                              {4, Encoder2::Direction::CCW, 1, 1, true},
                              {0, Encoder2::Direction::CCW, 0, 0, false},
                              {-4, Encoder2::Direction::CCW, -1, -1, false},
                              {-8, Encoder2::Direction::CCW, -2, -2, false}};
    runSequenceTest(steps, sizeof(steps) / sizeof(TestStep));
}

void test_direction_change_ccw_to_cw(void) {
    const TestStep steps[] = {{0, Encoder2::Direction::NONE, 0, 0, false},
                              {-4, Encoder2::Direction::CCW, -1, -1, false},
                              {-8, Encoder2::Direction::CCW, -2, -2, false},
                              {-4, Encoder2::Direction::CW, -1, -1, true},
                              {0, Encoder2::Direction::CW, 0, 0, false},
                              {4, Encoder2::Direction::CW, 1, 1, false},
                              {8, Encoder2::Direction::CW, 2, 2, false}};
    runSequenceTest(steps, sizeof(steps) / sizeof(TestStep));
}

int runUnityTests(void) {
    UNITY_BEGIN();
    RUN_TEST(test_initial_state);
    RUN_TEST(test_from_initial_state_to_cw_second_detent);
    RUN_TEST(test_from_initial_state_to_ccw_second_detent);
    RUN_TEST(test_fast_cw);
    RUN_TEST(test_fast_ccw);
    RUN_TEST(test_cw_bounce);
    RUN_TEST(test_ccw_bounce);
    RUN_TEST(test_direction_change_cw_to_ccw);
    RUN_TEST(test_direction_change_ccw_to_cw);
    return UNITY_END();
}

int main(void) {
    return runUnityTests();
}
