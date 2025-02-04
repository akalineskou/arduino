#include <unity.h>

void setUp() {
  // set stuff up here
}

void tearDown() {
  // clean stuff up here
}

void test_function_addition() {
  TEST_ASSERT_EQUAL(2, 1 + 1);
}

int runUnityTests() {
  UNITY_BEGIN();

  RUN_TEST(test_function_addition);

  return UNITY_END();
}

void setup() {
  runUnityTests();
}

void loop() {}
