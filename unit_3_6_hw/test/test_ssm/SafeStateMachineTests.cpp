#include <unity.h>

#include "SafeStateMachine.h"

static constexpr uint8_t CW = 0;
static constexpr uint8_t CCW = 1;

static constexpr uint8_t DIGIT_1 = 9;
static constexpr uint8_t DIGIT_2 = 8;
static constexpr uint8_t DIGIT_3 = 7;
static constexpr uint8_t DIGIT_1_POSITION = 0;
static constexpr uint8_t DIGIT_2_POSITION = 1;
static constexpr uint8_t DIGIT_3_POSITION = 2;
static constexpr uint8_t DIGIT_INITIAL_ZERO = 0;

static constexpr uint8_t WRONG_DIGIT_1 = 1;
static constexpr uint8_t WRONG_DIGIT_2 = 2;
static constexpr uint8_t WRONG_DIGIT_3 = 3;

static SafeStateMachine fsm;

void setUp(void) {
    SafeStateMachine::StateInfo stateInfo;

    stateInfo = fsm.setCode((uint8_t[]){DIGIT_1, DIGIT_2, DIGIT_3}, 3, 3);
    TEST_ASSERT_EQUAL(SafeStateMachine::IDLE, stateInfo.state);
    TEST_ASSERT_EQUAL(DIGIT_INITIAL_ZERO, stateInfo.enteredCode[0]);
    TEST_ASSERT_EQUAL(DIGIT_1_POSITION, stateInfo.currentDigitPosition);
    TEST_ASSERT_EQUAL(3, stateInfo.codeSize);
}

void tearDown(void) {
}

void test_first_confirmation_moves_from_idle_to_entering(void) {
    SafeStateMachine::StateInfo stateInfo = fsm.confirm();

    TEST_ASSERT_EQUAL(SafeStateMachine::ENTERING, stateInfo.state);
    TEST_ASSERT_EQUAL(DIGIT_INITIAL_ZERO, stateInfo.enteredCode[0]);
    TEST_ASSERT_EQUAL(DIGIT_1_POSITION, stateInfo.currentDigitPosition);
}

void test_second_confirmation_moves_to_next_digit(void) {
    fsm.confirm();

    SafeStateMachine::StateInfo stateInfo = fsm.confirm();

    TEST_ASSERT_EQUAL(SafeStateMachine::ENTERING, stateInfo.state);
    TEST_ASSERT_EQUAL(DIGIT_INITIAL_ZERO, stateInfo.enteredCode[1]);
    TEST_ASSERT_EQUAL(DIGIT_2_POSITION, stateInfo.currentDigitPosition);
}

void test_cw_rotation_increments_digit(void) {
    fsm.confirm();

    SafeStateMachine::StateInfo stateInfo = fsm.increment(1);
    TEST_ASSERT_EQUAL(SafeStateMachine::ENTERING, stateInfo.state);
    TEST_ASSERT_EQUAL(1, stateInfo.enteredCode[0]);
    TEST_ASSERT_EQUAL(DIGIT_1_POSITION, stateInfo.currentDigitPosition);
}

void test_ccw_rotation_increments_digit(void) {
    fsm.confirm();

    SafeStateMachine::StateInfo stateInfo = fsm.increment(1);
    TEST_ASSERT_EQUAL(SafeStateMachine::ENTERING, stateInfo.state);
    TEST_ASSERT_EQUAL(1, stateInfo.enteredCode[0]);
    TEST_ASSERT_EQUAL(DIGIT_1_POSITION, stateInfo.currentDigitPosition);
}

void test_cw_rotation_increments_digit_with_overflow(void) {
    fsm.confirm();
    SafeStateMachine::StateInfo stateInfo;

    stateInfo = fsm.increment(2);
    TEST_ASSERT_EQUAL(SafeStateMachine::ENTERING, stateInfo.state);
    TEST_ASSERT_EQUAL(2, stateInfo.enteredCode[0]);
    TEST_ASSERT_EQUAL(DIGIT_1_POSITION, stateInfo.currentDigitPosition);

    stateInfo = fsm.increment(6);
    TEST_ASSERT_EQUAL(SafeStateMachine::ENTERING, stateInfo.state);
    TEST_ASSERT_EQUAL(8, stateInfo.enteredCode[0]);
    TEST_ASSERT_EQUAL(DIGIT_1_POSITION, stateInfo.currentDigitPosition);

    stateInfo = fsm.increment(1);
    TEST_ASSERT_EQUAL(SafeStateMachine::ENTERING, stateInfo.state);
    TEST_ASSERT_EQUAL(9, stateInfo.enteredCode[0]);
    TEST_ASSERT_EQUAL(DIGIT_1_POSITION, stateInfo.currentDigitPosition);

    stateInfo = fsm.increment(1);
    TEST_ASSERT_EQUAL(SafeStateMachine::ENTERING, stateInfo.state);
    TEST_ASSERT_EQUAL(0, stateInfo.enteredCode[0]);
    TEST_ASSERT_EQUAL(DIGIT_1_POSITION, stateInfo.currentDigitPosition);

    stateInfo = fsm.increment(1);
    TEST_ASSERT_EQUAL(SafeStateMachine::ENTERING, stateInfo.state);
    TEST_ASSERT_EQUAL(1, stateInfo.enteredCode[0]);
    TEST_ASSERT_EQUAL(DIGIT_1_POSITION, stateInfo.currentDigitPosition);
}

void test_ccw_rotation_increments_digit_with_overflow(void) {
    fsm.confirm();
    SafeStateMachine::StateInfo stateInfo;

    stateInfo = fsm.increment(2);
    TEST_ASSERT_EQUAL(SafeStateMachine::ENTERING, stateInfo.state);
    TEST_ASSERT_EQUAL(2, stateInfo.enteredCode[0]);
    TEST_ASSERT_EQUAL(DIGIT_1_POSITION, stateInfo.currentDigitPosition);

    stateInfo = fsm.increment(6);
    TEST_ASSERT_EQUAL(SafeStateMachine::ENTERING, stateInfo.state);
    TEST_ASSERT_EQUAL(8, stateInfo.enteredCode[0]);
    TEST_ASSERT_EQUAL(DIGIT_1_POSITION, stateInfo.currentDigitPosition);

    stateInfo = fsm.increment(1);
    TEST_ASSERT_EQUAL(SafeStateMachine::ENTERING, stateInfo.state);
    TEST_ASSERT_EQUAL(9, stateInfo.enteredCode[0]);
    TEST_ASSERT_EQUAL(DIGIT_1_POSITION, stateInfo.currentDigitPosition);

    stateInfo = fsm.increment(1);
    TEST_ASSERT_EQUAL(SafeStateMachine::ENTERING, stateInfo.state);
    TEST_ASSERT_EQUAL(0, stateInfo.enteredCode[0]);
    TEST_ASSERT_EQUAL(DIGIT_1_POSITION, stateInfo.currentDigitPosition);

    stateInfo = fsm.increment(1);
    TEST_ASSERT_EQUAL(SafeStateMachine::ENTERING, stateInfo.state);
    TEST_ASSERT_EQUAL(1, stateInfo.enteredCode[0]);
    TEST_ASSERT_EQUAL(DIGIT_1_POSITION, stateInfo.currentDigitPosition);
}

void test_cw_to_ccw_change_direction_confirms_and_moves_to_next_digit(void) {
    fsm.confirm();
    const uint8_t DIGIT_1_CONFIRMED = 3;
    fsm.increment(DIGIT_1_CONFIRMED);

    SafeStateMachine::StateInfo stateInfo = fsm.confirm();
    TEST_ASSERT_EQUAL(SafeStateMachine::ENTERING, stateInfo.state);
    TEST_ASSERT_EQUAL(DIGIT_1_CONFIRMED, stateInfo.enteredCode[0]);
    TEST_ASSERT_EQUAL(DIGIT_INITIAL_ZERO, stateInfo.enteredCode[1]);
    TEST_ASSERT_EQUAL(DIGIT_2_POSITION, stateInfo.currentDigitPosition);
}

void test_ccw_to_cw_change_direction_confirms_and_moves_to_next_digit(void) {
    fsm.confirm();
    const uint8_t DIGIT_1_CONFIRMED = 3;
    fsm.increment(DIGIT_1_CONFIRMED);

    SafeStateMachine::StateInfo stateInfo = fsm.confirm();
    TEST_ASSERT_EQUAL(SafeStateMachine::ENTERING, stateInfo.state);
    TEST_ASSERT_EQUAL(DIGIT_1_CONFIRMED, stateInfo.enteredCode[0]);
    TEST_ASSERT_EQUAL(DIGIT_INITIAL_ZERO, stateInfo.enteredCode[1]);
    TEST_ASSERT_EQUAL(DIGIT_2_POSITION, stateInfo.currentDigitPosition);
}

void test_successful_code_entry_starting_from_cw(void) {
    fsm.confirm();
    fsm.increment(DIGIT_1);

    fsm.confirm();
    fsm.increment(DIGIT_2);

    fsm.confirm();
    fsm.increment(DIGIT_3);

    SafeStateMachine::StateInfo stateInfo = fsm.confirm();
    TEST_ASSERT_EQUAL(DIGIT_1, stateInfo.enteredCode[0]);
    TEST_ASSERT_EQUAL(DIGIT_2, stateInfo.enteredCode[1]);
    TEST_ASSERT_EQUAL(DIGIT_3, stateInfo.enteredCode[2]);
    TEST_ASSERT_EQUAL(SafeStateMachine::GRANTED, stateInfo.state);
}

void test_successful_code_entry_starting_from_ccw(void) {
    fsm.confirm();
    fsm.increment(DIGIT_1);

    fsm.confirm();
    fsm.increment(DIGIT_2);

    fsm.confirm();
    fsm.increment(DIGIT_3);

    SafeStateMachine::StateInfo stateInfo = fsm.confirm();
    TEST_ASSERT_EQUAL(DIGIT_1, stateInfo.enteredCode[0]);
    TEST_ASSERT_EQUAL(DIGIT_2, stateInfo.enteredCode[1]);
    TEST_ASSERT_EQUAL(DIGIT_3, stateInfo.enteredCode[2]);
    TEST_ASSERT_EQUAL(SafeStateMachine::GRANTED, stateInfo.state);
}

void test_failed_code_entry_starting_from_cw(void) {
    fsm.confirm();
    fsm.increment(WRONG_DIGIT_1);

    fsm.confirm();
    fsm.increment(WRONG_DIGIT_2);

    fsm.confirm();
    fsm.increment(WRONG_DIGIT_3);

    SafeStateMachine::StateInfo stateInfo = fsm.confirm();
    TEST_ASSERT_EQUAL(WRONG_DIGIT_1, stateInfo.enteredCode[0]);
    TEST_ASSERT_EQUAL(WRONG_DIGIT_2, stateInfo.enteredCode[1]);
    TEST_ASSERT_EQUAL(WRONG_DIGIT_3, stateInfo.enteredCode[2]);
    TEST_ASSERT_EQUAL(SafeStateMachine::WRONG_CODE, stateInfo.state);
}

void test_failed_code_entry_starting_from_ccw(void) {
    fsm.confirm();
    fsm.increment(WRONG_DIGIT_1);

    fsm.confirm();
    fsm.increment(WRONG_DIGIT_2);

    fsm.confirm();
    fsm.increment(WRONG_DIGIT_3);

    SafeStateMachine::StateInfo stateInfo = fsm.confirm();
    TEST_ASSERT_EQUAL(WRONG_DIGIT_1, stateInfo.enteredCode[0]);
    TEST_ASSERT_EQUAL(WRONG_DIGIT_2, stateInfo.enteredCode[1]);
    TEST_ASSERT_EQUAL(WRONG_DIGIT_3, stateInfo.enteredCode[2]);
    TEST_ASSERT_EQUAL(SafeStateMachine::WRONG_CODE, stateInfo.state);
}

void test_locked_state_ignores_encoder(void) {
    fsm.confirm();
    fsm.increment(WRONG_DIGIT_1);
    fsm.confirm();
    fsm.increment(WRONG_DIGIT_2);
    fsm.confirm();
    fsm.increment(WRONG_DIGIT_3);
    SafeStateMachine::StateInfo lockedState = fsm.confirm();

    TEST_ASSERT_EQUAL(SafeStateMachine::WRONG_CODE, lockedState.state);

    SafeStateMachine::StateInfo stateInfo = fsm.increment(5);
    TEST_ASSERT_EQUAL(SafeStateMachine::WRONG_CODE, stateInfo.state);
    TEST_ASSERT_EQUAL(WRONG_DIGIT_1, stateInfo.enteredCode[0]);
    TEST_ASSERT_EQUAL(WRONG_DIGIT_2, stateInfo.enteredCode[1]);
    TEST_ASSERT_EQUAL(WRONG_DIGIT_3, stateInfo.enteredCode[2]);
}

void test_granted_state_ignores_encoder(void) {
    fsm.confirm();
    fsm.increment(DIGIT_1);
    fsm.confirm();
    fsm.increment(DIGIT_2);
    fsm.confirm();
    fsm.increment(DIGIT_3);
    SafeStateMachine::StateInfo grantedState = fsm.confirm();

    TEST_ASSERT_EQUAL(SafeStateMachine::GRANTED, grantedState.state);

    SafeStateMachine::StateInfo stateInfo = fsm.increment(5);
    TEST_ASSERT_EQUAL(SafeStateMachine::GRANTED, stateInfo.state);
    TEST_ASSERT_EQUAL(DIGIT_1, stateInfo.enteredCode[0]);
    TEST_ASSERT_EQUAL(DIGIT_2, stateInfo.enteredCode[1]);
    TEST_ASSERT_EQUAL(DIGIT_3, stateInfo.enteredCode[2]);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_first_confirmation_moves_from_idle_to_entering);
    RUN_TEST(test_second_confirmation_moves_to_next_digit);
    RUN_TEST(test_cw_rotation_increments_digit);
    RUN_TEST(test_ccw_rotation_increments_digit);
    RUN_TEST(test_cw_rotation_increments_digit_with_overflow);
    RUN_TEST(test_ccw_rotation_increments_digit_with_overflow);
    RUN_TEST(test_cw_to_ccw_change_direction_confirms_and_moves_to_next_digit);
    RUN_TEST(test_ccw_to_cw_change_direction_confirms_and_moves_to_next_digit);
    RUN_TEST(test_successful_code_entry_starting_from_cw);
    RUN_TEST(test_successful_code_entry_starting_from_ccw);
    RUN_TEST(test_failed_code_entry_starting_from_cw);
    RUN_TEST(test_failed_code_entry_starting_from_ccw);
    RUN_TEST(test_locked_state_ignores_encoder);
    RUN_TEST(test_granted_state_ignores_encoder);
    return UNITY_END();
}
