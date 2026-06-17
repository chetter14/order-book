#include <gtest/gtest.h>
#include <array>
#include "order_book.h"

std::size_t getTotalOrdersCount(const OrderBook& ob) {
  std::size_t count = 0U;
  for (auto i = 0U; i < MAX_PRICE_VALUE; ++i) {
    count += ob.getOrdersAtPrice(i).size();
  }
  return count;
}

TEST(OrderBookGetOrders, NonExisting) {
  OrderBook ob;

  EXPECT_EQ(getTotalOrdersCount(ob), 0);
}

TEST(OrderBookBuy, BuyOnce) {
  OrderBook ob;
  InputOrder inputOrder{userId: 1, price: 20, amount: 5, type: OrderType::BUY};

  ob.applyOrder(inputOrder);

  auto&& orders = ob.getOrdersAtPrice(20);
  EXPECT_EQ(orders.size(), 1);

  auto&& order = orders.front();
  EXPECT_EQ(order.amount, inputOrder.amount);
  EXPECT_EQ(order.userId, inputOrder.userId);

  /* No more orders */
  EXPECT_EQ(getTotalOrdersCount(ob), 1);
}

TEST(OrderBookBuy, BuyDifferentPrices) {
  OrderBook ob;

  std::array<InputOrder, 3> inputOrders = {
      InputOrder{userId: 1, price: 24, amount: 5, type: OrderType::BUY},
      InputOrder{userId: 2, price: 18, amount: 10, type: OrderType::BUY},
      InputOrder{userId: 3, price: 32, amount: 8, type: OrderType::BUY}};

  for (auto&& inputOrder : inputOrders)
    ob.applyOrder(inputOrder);

  for (auto&& inputOrder : inputOrders) {
    auto&& orders = ob.getOrdersAtPrice(inputOrder.price);
    EXPECT_EQ(orders.size(), 1);

    auto&& order = orders.front();
    std::cout << order << "\n";
    EXPECT_EQ(order.amount, inputOrder.amount);
    EXPECT_EQ(order.userId, inputOrder.userId);
  }

  EXPECT_EQ(getTotalOrdersCount(ob), 3);
}

TEST(OrderBookBuy, BuyTheSamePrices) {
  OrderBook ob;

  std::array<InputOrder, 3> inputOrders = {
      InputOrder{userId: 1, price: 20, amount: 5, type: OrderType::BUY},
      InputOrder{userId: 2, price: 20, amount: 12, type: OrderType::BUY},
      InputOrder{userId: 3, price: 20, amount: 8, type: OrderType::BUY}};

  for (auto&& inputOrder : inputOrders)
    ob.applyOrder(inputOrder);

  auto&& orders = ob.getOrdersAtPrice(20);
  EXPECT_EQ(orders.size(), 3);

  EXPECT_EQ(orders.front().amount, inputOrders[0].amount);
  EXPECT_EQ(orders.front().userId, inputOrders[0].userId);

  EXPECT_EQ(orders.back().amount, inputOrders[2].amount);
  EXPECT_EQ(orders.back().userId, inputOrders[2].userId);

  EXPECT_EQ(getTotalOrdersCount(ob), 3);
}

TEST(OrderBookBuy, BuyTheSameAndDifferentPrices) {
  OrderBook ob;

  std::array<InputOrder, 6> inputOrders = {
      InputOrder{userId: 1, price: 20, amount: 5, type: OrderType::BUY},
      InputOrder{userId: 2, price: 22, amount: 12, type: OrderType::BUY},
      InputOrder{userId: 1, price: 22, amount: 8, type: OrderType::BUY},
      InputOrder{userId: 3, price: 24, amount: 10, type: OrderType::BUY},
      InputOrder{userId: 2, price: 23, amount: 6, type: OrderType::BUY},
      InputOrder{userId: 1, price: 25, amount: 20, type: OrderType::BUY}};

  for (auto&& inputOrder : inputOrders)
    ob.applyOrder(inputOrder);

  {
    auto&& orders = ob.getOrdersAtPrice(20);
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, inputOrders[0].amount);
    EXPECT_EQ(orders.front().userId, inputOrders[0].userId);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(22);
    EXPECT_EQ(orders.size(), 2);

    EXPECT_EQ(orders.front().amount, inputOrders[1].amount);
    EXPECT_EQ(orders.front().userId, inputOrders[1].userId);

    EXPECT_EQ(orders.back().amount, inputOrders[2].amount);
    EXPECT_EQ(orders.back().userId, inputOrders[2].userId);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(24);
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, inputOrders[3].amount);
    EXPECT_EQ(orders.front().userId, inputOrders[3].userId);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(23);
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, inputOrders[4].amount);
    EXPECT_EQ(orders.front().userId, inputOrders[4].userId);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(25);
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, inputOrders[5].amount);
    EXPECT_EQ(orders.front().userId, inputOrders[5].userId);
  }

  EXPECT_EQ(getTotalOrdersCount(ob), 6);
}

