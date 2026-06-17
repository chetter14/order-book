#include <gtest/gtest.h>
#include <array>
#include "order_book.h"

TEST(OrderBookGetOrders, NonExisting) {
  OrderBook ob;

  auto&& nonExistingOrders = ob.getOrdersAtPrice(10);
  ASSERT_EQ(nonExistingOrders.size(), 0);
}

TEST(OrderBookBuy, BuyOnce) {
  OrderBook ob;
  InputOrder inputOrder{userId: 1, price: 20, amount: 5, type: OrderType::BUY};

  ob.applyOrder(inputOrder);

  auto&& orders = ob.getOrdersAtPrice(inputOrder.price);
  ASSERT_EQ(orders.size(), 1);

  auto&& order = orders.front();
  ASSERT_EQ(order.amount, inputOrder.amount);
  ASSERT_EQ(order.userId, inputOrder.userId);

  /* No other orders were modified after */
  auto&& nonExistingOrders = ob.getOrdersAtPrice(inputOrder.price + 10);
  ASSERT_EQ(nonExistingOrders.size(), 0);
}

TEST(OrderBookBuy, BuyDifferentPrices) {
  OrderBook ob;

  /* Prices have to differ, at least, by 2 */
  std::array<InputOrder, 3> inputOrders = {
      InputOrder{userId: 1, price: 24, amount: 5, type: OrderType::BUY},
      InputOrder{userId: 2, price: 18, amount: 10, type: OrderType::BUY},
      InputOrder{userId: 3, price: 32, amount: 8, type: OrderType::BUY}};

  for (auto&& inputOrder : inputOrders)
    ob.applyOrder(inputOrder);

  for (auto&& inputOrder : inputOrders) {
    auto&& orders = ob.getOrdersAtPrice(inputOrder.price);
    ASSERT_EQ(orders.size(), 1);

    auto&& order = orders.front();
    std::cout << order << "\n";
    ASSERT_EQ(order.amount, inputOrder.amount);
    ASSERT_EQ(order.userId, inputOrder.userId);

    ASSERT_EQ(ob.getOrdersAtPrice(inputOrder.price - 1).size(), 0);
    ASSERT_EQ(ob.getOrdersAtPrice(inputOrder.price + 1).size(), 0);
  }
}

TEST(OrderBookBuy, BuyTheSamePrices) {
  OrderBook ob;

  std::array<InputOrder, 3> inputOrders = {
      InputOrder{userId: 1, price: 20, amount: 5, type: OrderType::BUY},
      InputOrder{userId: 2, price: 20, amount: 12, type: OrderType::BUY},
      InputOrder{userId: 3, price: 20, amount: 8, type: OrderType::BUY}};

  for (auto&& inputOrder : inputOrders)
    ob.applyOrder(inputOrder);

  auto&& orders = ob.getOrdersAtPrice(inputOrders[0].price);
  ASSERT_EQ(orders.size(), 3);

  ASSERT_EQ(orders.front().amount, inputOrders[0].amount);
  ASSERT_EQ(orders.front().userId, inputOrders[0].userId);

  ASSERT_EQ(orders.back().amount, inputOrders[2].amount);
  ASSERT_EQ(orders.back().userId, inputOrders[2].userId);
}

