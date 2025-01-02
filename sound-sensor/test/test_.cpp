#include <unity.h>

void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}

void test_function_addition(void) {
  TEST_ASSERT_EQUAL(2, 1 + 1);
}

int runUnityTests(void) {
  UNITY_BEGIN();

  RUN_TEST(test_function_addition);

  return UNITY_END();
}

void setup() {
  runUnityTests();
}

void loop() {}
