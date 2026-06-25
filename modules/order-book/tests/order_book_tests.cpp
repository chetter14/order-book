#include <gtest/gtest.h>
#include <algorithm>
#include <array>
#include "order_book.h"

/**
 * @brief Returns a total number of shares at a specific price
 * 
 * @param ob the order book
 * @param price price with orders at
 * @return number of shares present 
 */
unsigned int restingAt(const OrderBook& ob, Price price) {
  auto orders = ob.getOrdersAtPrice(price);
  if (!orders.has_value()) {
    return -1;
  }

  unsigned int total = 0U;
  std::for_each(orders.value().cbegin(), orders.value().cend(),
                [&](const Order& order) { total += order.amount; });

  return total;
}

constexpr OrderType oppositeOrderType(OrderType type) {
  switch (type) {
    case OrderType::BUY:
      return OrderType::SELL;
    case OrderType::SELL:
      return OrderType::BUY;
  }
}

TEST(OrderBookGetOrders, NonExisting) {
  OrderBook ob;
  EXPECT_EQ(ob.getTotalOrdersCount(), 0);
}

class OrderBookBuySellSeparate : public ::testing::TestWithParam<OrderType> {};

TEST_P(OrderBookBuySellSeparate, Once) {
  OrderType orderType = GetParam();

  OrderBook ob;
  InputOrder inputOrder{
      .userId = 1, .price = 20, .amount = 5, .type = orderType};

  ob.applyOrder(inputOrder);

  auto orders = ob.getOrdersAtPrice(20).value();
  EXPECT_EQ(orders.size(), 1);

  const auto& order = orders.front();
  EXPECT_EQ(order.amount, inputOrder.amount);
  EXPECT_EQ(order.userId, inputOrder.userId);

  /* No more orders */
  EXPECT_EQ(ob.getTotalOrdersCount(), 1);
}

TEST_P(OrderBookBuySellSeparate, DifferentPrices) {
  OrderType orderType = GetParam();

  OrderBook ob;

  std::array<InputOrder, 3> inputOrders = {
      InputOrder{userId: 1, price: 24, amount: 5, type: orderType},
      InputOrder{userId: 2, price: 18, amount: 10, type: orderType},
      InputOrder{userId: 3, price: 32, amount: 8, type: orderType}};

  for (auto&& inputOrder : inputOrders)
    ob.applyOrder(inputOrder);

  for (auto&& inputOrder : inputOrders) {
    auto orders = ob.getOrdersAtPrice(inputOrder.price).value();
    EXPECT_EQ(orders.size(), 1);

    const auto& order = orders.front();
    EXPECT_EQ(order.amount, inputOrder.amount);
    EXPECT_EQ(order.userId, inputOrder.userId);
  }

  EXPECT_EQ(ob.getTotalOrdersCount(), 3);
}

TEST_P(OrderBookBuySellSeparate, TheSamePrice) {
  OrderType orderType = GetParam();

  OrderBook ob;

  std::array<InputOrder, 3> inputOrders = {
      InputOrder{userId: 1, price: 20, amount: 5, type: orderType},
      InputOrder{userId: 2, price: 20, amount: 12, type: orderType},
      InputOrder{userId: 3, price: 20, amount: 8, type: orderType}};

  for (auto&& inputOrder : inputOrders)
    ob.applyOrder(inputOrder);

  auto orders = ob.getOrdersAtPrice(20).value();
  EXPECT_EQ(orders.size(), 3);
  EXPECT_EQ(restingAt(ob, 20), 25);

  EXPECT_EQ(orders.front().amount, inputOrders[0].amount);
  EXPECT_EQ(orders.front().userId, inputOrders[0].userId);

  EXPECT_EQ(orders.back().amount, inputOrders[2].amount);
  EXPECT_EQ(orders.back().userId, inputOrders[2].userId);

  EXPECT_EQ(ob.getTotalOrdersCount(), 3);
}

TEST_P(OrderBookBuySellSeparate, TheSameAndDifferentPrices) {
  OrderType orderType = GetParam();

  OrderBook ob;

  std::array<InputOrder, 6> inputOrders = {
      InputOrder{userId: 1, price: 20, amount: 5, type: orderType},
      InputOrder{userId: 2, price: 22, amount: 12, type: orderType},
      InputOrder{userId: 1, price: 22, amount: 8, type: orderType},
      InputOrder{userId: 3, price: 24, amount: 10, type: orderType},
      InputOrder{userId: 2, price: 22, amount: 6, type: orderType},
      InputOrder{userId: 1, price: 25, amount: 20, type: orderType}};

  for (auto&& inputOrder : inputOrders)
    ob.applyOrder(inputOrder);

  {
    auto orders = ob.getOrdersAtPrice(20).value();
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, inputOrders[0].amount);
    EXPECT_EQ(orders.front().userId, inputOrders[0].userId);
  }

  {
    auto orders = ob.getOrdersAtPrice(22).value();
    EXPECT_EQ(orders.size(), 3);
    EXPECT_EQ(restingAt(ob, 22), 26);

    EXPECT_EQ(orders.front().amount, inputOrders[1].amount);
    EXPECT_EQ(orders.front().userId, inputOrders[1].userId);

    EXPECT_EQ(orders.back().amount, inputOrders[4].amount);
    EXPECT_EQ(orders.back().userId, inputOrders[4].userId);
  }

  {
    auto orders = ob.getOrdersAtPrice(24).value();
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, inputOrders[3].amount);
    EXPECT_EQ(orders.front().userId, inputOrders[3].userId);
  }

  {
    auto orders = ob.getOrdersAtPrice(25).value();
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, inputOrders[5].amount);
    EXPECT_EQ(orders.front().userId, inputOrders[5].userId);
  }

  EXPECT_EQ(ob.getTotalOrdersCount(), 6);
}

INSTANTIATE_TEST_SUITE_P(, OrderBookBuySellSeparate,
                         ::testing::Values(OrderType::BUY, OrderType::SELL));

TEST(OrderBookBuySellMixed, NoOverlap) {
  OrderBook ob;

  ob.applyOrder(buy(1, 15, 5));
  ob.applyOrder(sell(2, 25, 10));
  ob.applyOrder(buy(3, 20, 20));
  ob.applyOrder(sell(4, 30, 12));

  EXPECT_EQ(restingAt(ob, 15), 5);
  EXPECT_EQ(restingAt(ob, 25), 10);
  EXPECT_EQ(restingAt(ob, 20), 20);
  EXPECT_EQ(restingAt(ob, 30), 12);
  EXPECT_EQ(ob.getTotalOrdersCount(), 4);
}

TEST(OrderBookMixedBuyFirst, PartialFillLeavesResidualVisibleToNextOrder) {
  OrderBook ob;

  ob.applyOrder(buy(1, 100, 200));
  ob.applyOrder(sell(2, 100, 50));

  EXPECT_EQ(restingAt(ob, 100), 150);
  EXPECT_EQ(ob.getTotalOrdersCount(), 1);

  ob.applyOrder(sell(3, 100, 100));

  EXPECT_EQ(restingAt(ob, 100), 50);
  EXPECT_EQ(ob.getTotalOrdersCount(), 1);
  EXPECT_EQ(ob.getOrdersAtPrice(100).value().at(0).userId, 1);
}

TEST(OrderBookMixedSellFirst, PartialFillLeavesResidualVisibleToNextOrder) {
  OrderBook ob;

  ob.applyOrder(sell(1, 100, 200));
  ob.applyOrder(buy(2, 100, 50));

  EXPECT_EQ(restingAt(ob, 100), 150);
  EXPECT_EQ(ob.getTotalOrdersCount(), 1);

  ob.applyOrder(buy(3, 100, 100));

  EXPECT_EQ(restingAt(ob, 100), 50);
  EXPECT_EQ(ob.getTotalOrdersCount(), 1);
  EXPECT_EQ(ob.getOrdersAtPrice(100).value().at(0).userId, 1);
}

TEST(OrderBookMixedBuyFirst, ExactFillEmptiesLevel) {
  OrderBook ob;

  ob.applyOrder(buy(1, 100, 50));
  ob.applyOrder(sell(2, 100, 50));

  EXPECT_EQ(restingAt(ob, 100), 0);
  EXPECT_EQ(ob.getTotalOrdersCount(), 0);

  ob.applyOrder(sell(3, 100, 30));
  EXPECT_EQ(restingAt(ob, 100), 30);
  EXPECT_EQ(ob.getTotalOrdersCount(), 1);
}

TEST(OrderBookMixedSellFirst, ExactFillEmptiesLevel) {
  OrderBook ob;

  ob.applyOrder(sell(1, 100, 50));
  ob.applyOrder(buy(2, 100, 50));

  EXPECT_EQ(restingAt(ob, 100), 0);
  EXPECT_EQ(ob.getTotalOrdersCount(), 0);

  ob.applyOrder(buy(3, 100, 30));
  EXPECT_EQ(restingAt(ob, 100), 30);
  EXPECT_EQ(ob.getTotalOrdersCount(), 1);
}

TEST(OrderBookMixedBuyFirst, SellSweepsMultipleBidLevels) {
  OrderBook ob;

  ob.applyOrder(buy(1, 100, 30));
  ob.applyOrder(buy(2, 101, 30));
  ob.applyOrder(buy(3, 102, 30));

  EXPECT_EQ(ob.getTotalOrdersCount(), 3);

  ob.applyOrder(sell(4, 100, 75));

  EXPECT_EQ(restingAt(ob, 102), 0);
  EXPECT_EQ(restingAt(ob, 101), 0);
  EXPECT_EQ(restingAt(ob, 100), 15);
  EXPECT_EQ(ob.getTotalOrdersCount(), 1);
  EXPECT_EQ(ob.getOrdersAtPrice(100).value().at(0).userId, 1);

  ob.applyOrder(buy(5, 101, 20));
  EXPECT_EQ(restingAt(ob, 101), 20);
  EXPECT_EQ(ob.getTotalOrdersCount(), 2);
}

TEST(OrderBookMixedSellFirst, BuySweepsMultipleAskLevels) {
  OrderBook ob;

  ob.applyOrder(sell(1, 100, 30));
  ob.applyOrder(sell(2, 101, 30));
  ob.applyOrder(sell(3, 102, 30));

  EXPECT_EQ(ob.getTotalOrdersCount(), 3);

  ob.applyOrder(buy(4, 102, 75));

  EXPECT_EQ(restingAt(ob, 102), 15);
  EXPECT_EQ(restingAt(ob, 101), 0);
  EXPECT_EQ(restingAt(ob, 100), 0);
  EXPECT_EQ(ob.getTotalOrdersCount(), 1);
  EXPECT_EQ(ob.getOrdersAtPrice(102).value().at(0).userId, 3);

  ob.applyOrder(sell(5, 101, 20));
  EXPECT_EQ(restingAt(ob, 101), 20);
  EXPECT_EQ(ob.getTotalOrdersCount(), 2);
}

TEST(OrderBookMixedBuyFirst, TimePriorityFIFO) {
  OrderBook ob;

  ob.applyOrder(buy(1, 100, 40));
  ob.applyOrder(buy(2, 100, 40));
  ob.applyOrder(sell(3, 100, 40));

  EXPECT_EQ(restingAt(ob, 100), 40);
  EXPECT_EQ(ob.getTotalOrdersCount(), 1);
  EXPECT_EQ(ob.getOrdersAtPrice(100).value().at(0).userId, 2);
}

TEST(OrderBookMixedSellFirst, TimePriorityFIFO) {
  OrderBook ob;

  ob.applyOrder(sell(1, 100, 40));
  ob.applyOrder(sell(2, 100, 40));
  ob.applyOrder(buy(3, 100, 40));

  EXPECT_EQ(restingAt(ob, 100), 40);
  EXPECT_EQ(ob.getTotalOrdersCount(), 1);
  EXPECT_EQ(ob.getOrdersAtPrice(100).value().at(0).userId, 2);
}

TEST(OrderBookMixedBuyFirst, CrossingSellFullyMatches) {
  OrderBook ob;

  ob.applyOrder(buy(1, 105, 50));
  ob.applyOrder(sell(2, 100, 50));

  EXPECT_EQ(restingAt(ob, 100), 0);
  EXPECT_EQ(restingAt(ob, 105), 0);
  EXPECT_EQ(ob.getTotalOrdersCount(), 0);
}

TEST(OrderBookMixedSellFirst, CrossingBuyFullyMatches) {
  OrderBook ob;

  ob.applyOrder(sell(1, 100, 50));
  ob.applyOrder(buy(2, 105, 50));

  EXPECT_EQ(restingAt(ob, 100), 0);
  EXPECT_EQ(restingAt(ob, 105), 0);
  EXPECT_EQ(ob.getTotalOrdersCount(), 0);
}

TEST(OrderBookMixedBuyFirst, TwoSidedBookThenCross) {
  OrderBook ob;

  ob.applyOrder(buy(1, 98, 40));
  ob.applyOrder(buy(2, 99, 50));
  ob.applyOrder(sell(3, 102, 60));
  ob.applyOrder(sell(4, 101, 70));

  EXPECT_EQ(restingAt(ob, 99), 50);
  EXPECT_EQ(restingAt(ob, 101), 70);
  EXPECT_EQ(ob.getTotalOrdersCount(), 4);

  ob.applyOrder(sell(5, 99, 50));

  EXPECT_EQ(restingAt(ob, 99), 0);
  EXPECT_EQ(ob.getTotalOrdersCount(), 3);

  ob.applyOrder(sell(6, 98, 15));

  EXPECT_EQ(restingAt(ob, 98), 25);
  EXPECT_EQ(ob.getTotalOrdersCount(), 3);
}

TEST(OrderBookMixedSellFirst, TwoSidedBookThenCross) {
  OrderBook ob;

  ob.applyOrder(sell(1, 102, 40));
  ob.applyOrder(sell(2, 101, 50));
  ob.applyOrder(buy(3, 98, 70));
  ob.applyOrder(buy(4, 99, 60));

  EXPECT_EQ(restingAt(ob, 99), 60);
  EXPECT_EQ(restingAt(ob, 101), 50);
  EXPECT_EQ(ob.getTotalOrdersCount(), 4);

  ob.applyOrder(buy(5, 101, 50));

  EXPECT_EQ(restingAt(ob, 101), 0);
  EXPECT_EQ(ob.getTotalOrdersCount(), 3);

  ob.applyOrder(buy(6, 102, 15));

  EXPECT_EQ(restingAt(ob, 102), 25);
  EXPECT_EQ(ob.getTotalOrdersCount(), 3);
}

class OrderBookEdgeCases : public ::testing::TestWithParam<std::size_t> {};

TEST_P(OrderBookEdgeCases, BuyFirstPriceAtValue) {
  std::size_t priceValue = GetParam();

  OrderBook ob;

  ob.applyOrder(buy(1, priceValue, 40));

  EXPECT_EQ(restingAt(ob, priceValue), 40);
  EXPECT_EQ(ob.getTotalOrdersCount(), 1);

  ob.applyOrder(sell(2, priceValue, 10));

  EXPECT_EQ(restingAt(ob, priceValue), 30);
  EXPECT_EQ(ob.getTotalOrdersCount(), 1);

  ob.applyOrder(sell(3, priceValue, 30));

  EXPECT_EQ(restingAt(ob, priceValue), 0);
  EXPECT_EQ(ob.getTotalOrdersCount(), 0);

  ob.applyOrder(buy(4, priceValue, 40));

  EXPECT_EQ(restingAt(ob, priceValue), 40);
  EXPECT_EQ(ob.getTotalOrdersCount(), 1);

  ob.applyOrder(sell(5, priceValue, 40));

  EXPECT_EQ(restingAt(ob, priceValue), 0);
  EXPECT_EQ(ob.getTotalOrdersCount(), 0);
}

TEST_P(OrderBookEdgeCases, SellFirstPriceAtValue) {
  std::size_t priceValue = GetParam();

  OrderBook ob;

  ob.applyOrder(sell(1, priceValue, 40));

  EXPECT_EQ(restingAt(ob, priceValue), 40);
  EXPECT_EQ(ob.getTotalOrdersCount(), 1);

  ob.applyOrder(buy(2, priceValue, 10));

  EXPECT_EQ(restingAt(ob, priceValue), 30);
  EXPECT_EQ(ob.getTotalOrdersCount(), 1);

  ob.applyOrder(buy(3, priceValue, 30));

  EXPECT_EQ(restingAt(ob, priceValue), 0);
  EXPECT_EQ(ob.getTotalOrdersCount(), 0);

  ob.applyOrder(sell(4, priceValue, 40));

  EXPECT_EQ(restingAt(ob, priceValue), 40);
  EXPECT_EQ(ob.getTotalOrdersCount(), 1);

  ob.applyOrder(buy(5, priceValue, 40));

  EXPECT_EQ(restingAt(ob, priceValue), 0);
  EXPECT_EQ(ob.getTotalOrdersCount(), 0);
}

TEST_P(OrderBookEdgeCases, PriceOutOfRange) {
  OrderBook ob;

  auto result = ob.applyOrder(buy(1, MAX_PRICE_VALUE + 1, 40));

  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), OrderBookError::PRICE_OUT_OF_RANGE);

  EXPECT_EQ(restingAt(ob, MAX_PRICE_VALUE + 1), -1);
  EXPECT_EQ(ob.getTotalOrdersCount(), 0);

  result = ob.applyOrder(sell(2, 0, 20));

  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), OrderBookError::PRICE_OUT_OF_RANGE);

  EXPECT_EQ(restingAt(ob, -1), -1);
  EXPECT_EQ(ob.getTotalOrdersCount(), 0);
}

TEST_P(OrderBookEdgeCases, GetOrdersByInvalidPrice) {
  OrderBook ob;

  auto orders = ob.getOrdersAtPrice(0);
  EXPECT_FALSE(orders.has_value());

  orders = ob.getOrdersAtPrice(MAX_PRICE_VALUE + 1);
  EXPECT_FALSE(orders.has_value());
}

INSTANTIATE_TEST_SUITE_P(, OrderBookEdgeCases,
                         ::testing::Values(MIN_PRICE_VALUE, MAX_PRICE_VALUE));
