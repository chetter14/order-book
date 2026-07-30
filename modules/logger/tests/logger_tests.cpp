#include <gtest/gtest.h>
#include <string>
#include "logger.h"

TEST(LoggerFormat, ProducesExpectedLine) {
  using namespace std::chrono;
  const sys_time<milliseconds> when{sys_days{2026y / June / 15} + hours{12} +
                                    minutes{7} + milliseconds{317}};

  ob::ExecutedOrder order{.buyer = 1,
                          .seller = 2,
                          .type = ob::OrderType::BUY,
                          .price = 33,
                          .amount = 44};

  EXPECT_EQ(ob_logger::formatExecutedOrder(order, when),
            "[2026-06-15 12:07:00.317 UTC] OPERATION=BUY, BUYER=1, SELLER=2, "
            "PRICE=33, AMOUNT=44");
}
