#include <gtest/gtest.h>

#include "utils.h"

TEST(UtilsTest, Factorial) {
  ASSERT_EQ(utils::factorial(1u), 1);
  ASSERT_EQ(utils::factorial(2u), 2);
  ASSERT_EQ(utils::factorial(3u), 6);
  ASSERT_EQ(utils::factorial(4u), 24);
}