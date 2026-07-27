#include <gtest/gtest.h>
#include "logger.h"

/* TO-DO: proper tests are required */

TEST(Logger, TestLogging) {
  ob_logger::Logger logger{"test_file.txt"};

  logger.recordExecutedOrder(
      ob_logger::ExecutedOrder{1, 2, ob::OrderType::BUY, 120, 5});

  ASSERT_EQ(1, 1);
}
