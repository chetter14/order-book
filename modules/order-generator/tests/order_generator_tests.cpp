#include <gtest/gtest.h>
#include "order_generator.h"

TEST(OrderGenerator, UserIdIsAlwaysInRange) {
  og::OrderGenerator orderGen(0);

  for (std::size_t i = 0; i < 100000; ++i) {
    const auto ord = orderGen.generateOrder();
    ASSERT_GE(ord.userId, og::MIN_USER_ID);
    ASSERT_LE(ord.userId, og::MAX_USER_ID);
  }
}

TEST(OrderGenerator, PriceIsAlwaysInRange) {
  og::OrderGenerator orderGen(0);

  for (std::size_t i = 0; i < 1000000; ++i) {
    const auto ord = orderGen.generateOrder();
    ASSERT_GE(ord.price, ob::MIN_PRICE_VALUE);
    ASSERT_LE(ord.price, ob::MAX_PRICE_VALUE);
  }
}

TEST(OrderGenerator, AmountIsAlwaysInRange) {
  og::OrderGenerator orderGen(0);

  for (std::size_t i = 0; i < 100000; ++i) {
    const auto ord = orderGen.generateOrder();
    ASSERT_GE(ord.amount, og::MIN_AMOUNT_VALUE);
    ASSERT_LE(ord.amount, og::MAX_AMOUNT_VALUE);
  }
}

TEST(OrderGenerator, OrderTypeIsAlwaysInRange) {
  og::OrderGenerator orderGen(0);

  for (std::size_t i = 0; i < 10000; ++i) {
    const auto ord = orderGen.generateOrder();
    ASSERT_TRUE(ord.type == ob::OrderType::BUY ||
                ord.type == ob::OrderType::SELL);
  }
}

TEST(OrderGenerator, SameSeedProducesSameSequence) {
  og::OrderGenerator a(31), b(31);

  for (std::size_t i = 0; i < 10000; ++i) {
    const auto orderA = a.generateOrder();
    const auto orderB = b.generateOrder();

    ASSERT_EQ(orderA.userId, orderB.userId);
    ASSERT_EQ(orderA.price, orderB.price);
    ASSERT_EQ(orderA.amount, orderB.amount);
    ASSERT_EQ(orderA.type, orderB.type);
  }
}

TEST(OrderGenerator, PriceIsNearToMidPrice) {
  og::OrderGenerator orderGen(0);
  constexpr std::size_t N = 1000000;
  double totalPrice = 0;

  for (std::size_t i = 0; i < N; ++i) {
    totalPrice += orderGen.generateOrder().price;
  }

  EXPECT_NEAR(totalPrice / N, 5000.0, 12.5);
}

TEST(OrderGenerator, BothOrderTypesAreGenerated) {
  og::OrderGenerator orderGen(0);
  constexpr std::size_t N = 100000;
  std::size_t buys = 0;

  for (std::size_t i = 0; i < N; ++i) {
    if (orderGen.generateOrder().type == ob::OrderType::BUY)
      ++buys;
  }

  EXPECT_NEAR(static_cast<double>(buys) / N, 0.5, 0.01);
}
