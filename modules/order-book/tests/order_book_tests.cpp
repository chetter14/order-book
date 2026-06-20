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

class OrderBookBuySellSeparate : public ::testing::TestWithParam<OrderType> {};

TEST_P(OrderBookBuySellSeparate, Once) {
  OrderType orderType = GetParam();

  OrderBook ob;
  InputOrder inputOrder{userId: 1, price: 20, amount: 5, type: orderType};

  ob.applyOrder(inputOrder);

  auto&& orders = ob.getOrdersAtPrice(20);
  EXPECT_EQ(orders.size(), 1);

  auto&& order = orders.front();
  EXPECT_EQ(order.amount, inputOrder.amount);
  EXPECT_EQ(order.userId, inputOrder.userId);

  /* No more orders */
  EXPECT_EQ(getTotalOrdersCount(ob), 1);
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
    auto&& orders = ob.getOrdersAtPrice(inputOrder.price);
    EXPECT_EQ(orders.size(), 1);

    auto&& order = orders.front();
    std::cout << order << "\n";
    EXPECT_EQ(order.amount, inputOrder.amount);
    EXPECT_EQ(order.userId, inputOrder.userId);
  }

  EXPECT_EQ(getTotalOrdersCount(ob), 3);
}

TEST_P(OrderBookBuySellSeparate, TheSamePrices) {
  OrderType orderType = GetParam();

  OrderBook ob;

  std::array<InputOrder, 3> inputOrders = {
      InputOrder{userId: 1, price: 20, amount: 5, type: orderType},
      InputOrder{userId: 2, price: 20, amount: 12, type: orderType},
      InputOrder{userId: 3, price: 20, amount: 8, type: orderType}};

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

TEST_P(OrderBookBuySellSeparate, TheSameAndDifferentPrices) {
  OrderType orderType = GetParam();

  OrderBook ob;

  std::array<InputOrder, 6> inputOrders = {
      InputOrder{userId: 1, price: 20, amount: 5, type: orderType},
      InputOrder{userId: 2, price: 22, amount: 12, type: orderType},
      InputOrder{userId: 1, price: 22, amount: 8, type: orderType},
      InputOrder{userId: 3, price: 24, amount: 10, type: orderType},
      InputOrder{userId: 2, price: 23, amount: 6, type: orderType},
      InputOrder{userId: 1, price: 25, amount: 20, type: orderType}};

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

INSTANTIATE_TEST_SUITE_P(, OrderBookBuySellSeparate,
                         ::testing::Values(OrderType::BUY, OrderType::SELL));

TEST(OrderBookBuySellMixed, NoOverlap) {
  OrderBook ob;

  std::array<InputOrder, 4> inputOrders = {
      InputOrder{userId: 1, price: 15, amount: 5, type: OrderType::BUY},
      InputOrder{userId: 3, price: 25, amount: 10, type: OrderType::SELL},
      InputOrder{userId: 2, price: 20, amount: 20, type: OrderType::BUY},
      InputOrder{userId: 4, price: 30, amount: 12, type: OrderType::SELL}};

  for (auto&& order : inputOrders)
    ob.applyOrder(order);

  EXPECT_EQ(getTotalOrdersCount(ob), 4);

  {
    auto&& orders = ob.getOrdersAtPrice(20);
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, inputOrders[2].amount);
    EXPECT_EQ(orders.front().userId, inputOrders[2].userId);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(15);
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, inputOrders[0].amount);
    EXPECT_EQ(orders.front().userId, inputOrders[0].userId);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(30);
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, inputOrders[3].amount);
    EXPECT_EQ(orders.front().userId, inputOrders[3].userId);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(25);
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, inputOrders[1].amount);
    EXPECT_EQ(orders.front().userId, inputOrders[1].userId);
  }
}

TEST(OrderBookBuySellMixed, EnoughSellersForBid1) {
  OrderBook ob;

  std::array<InputOrder, 4> inputOrders = {
      InputOrder{userId: 1, price: 15, amount: 5, type: OrderType::SELL},
      InputOrder{userId: 3, price: 25, amount: 10, type: OrderType::SELL},
      InputOrder{userId: 2, price: 20, amount: 8, type: OrderType::SELL},
      InputOrder{userId: 4, price: 30, amount: 25, type: OrderType::BUY}};

  for (auto&& order : inputOrders)
    ob.applyOrder(order);

  EXPECT_EQ(getTotalOrdersCount(ob), 1);

  {
    auto&& orders = ob.getOrdersAtPrice(20);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(15);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(25);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(30);
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, 2);
    EXPECT_EQ(orders.front().userId, inputOrders[3].userId);
  }
}

TEST(OrderBookBuySellMixed, EnoughSellersForBid2) {
  OrderBook ob;

  std::array<InputOrder, 4> inputOrders = {
      InputOrder{userId: 1, price: 15, amount: 5, type: OrderType::SELL},
      InputOrder{userId: 3, price: 20, amount: 10, type: OrderType::SELL},
      InputOrder{userId: 2, price: 30, amount: 14, type: OrderType::SELL},
      InputOrder{userId: 4, price: 25, amount: 15, type: OrderType::BUY}};

  for (auto&& order : inputOrders)
    ob.applyOrder(order);

  EXPECT_EQ(getTotalOrdersCount(ob), 1);

  {
    auto&& orders = ob.getOrdersAtPrice(20);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(15);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(25);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(30);
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, inputOrders[2].amount);
    EXPECT_EQ(orders.front().userId, inputOrders[2].userId);
  }
}

TEST(OrderBookBuySellMixed, NotEnoughSellersForBid1) {
  OrderBook ob;

  std::array<InputOrder, 4> inputOrders = {
      InputOrder{userId: 1, price: 15, amount: 5, type: OrderType::SELL},
      InputOrder{userId: 3, price: 20, amount: 10, type: OrderType::SELL},
      InputOrder{userId: 2, price: 30, amount: 14, type: OrderType::SELL},
      InputOrder{userId: 4, price: 25, amount: 22, type: OrderType::BUY}};

  for (auto&& order : inputOrders)
    ob.applyOrder(order);

  EXPECT_EQ(getTotalOrdersCount(ob), 2);

  {
    auto&& orders = ob.getOrdersAtPrice(20);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(15);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(25);
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, 7);
    EXPECT_EQ(orders.front().userId, inputOrders[3].userId);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(30);
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, inputOrders[2].amount);
    EXPECT_EQ(orders.front().userId, inputOrders[2].userId);
  }
}

TEST(OrderBookBuySellMixed, NotEnoughSellersForBid2) {
  OrderBook ob;

  std::array<InputOrder, 5> inputOrders = {
      InputOrder{userId: 1, price: 15, amount: 5, type: OrderType::SELL},
      InputOrder{userId: 3, price: 20, amount: 10, type: OrderType::SELL},
      InputOrder{userId: 2, price: 30, amount: 14, type: OrderType::SELL},
      InputOrder{userId: 4, price: 25, amount: 22, type: OrderType::BUY},
      InputOrder{userId: 5, price: 18, amount: 3, type: OrderType::SELL}};

  for (auto&& order : inputOrders)
    ob.applyOrder(order);

  EXPECT_EQ(getTotalOrdersCount(ob), 2);

  {
    auto&& orders = ob.getOrdersAtPrice(20);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(15);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(18);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(25);
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, 4);
    EXPECT_EQ(orders.front().userId, inputOrders[3].userId);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(30);
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, inputOrders[2].amount);
    EXPECT_EQ(orders.front().userId, inputOrders[2].userId);
  }
}

/**
 * @brief There is not enough sellers at first, then an appropriate offer is added, and 
 * the bid is completed
 * 
 */
TEST(OrderBookBuySellMixed, CompleteBidLater1) {
  OrderBook ob;

  std::array<InputOrder, 5> inputOrders = {
      InputOrder{userId: 1, price: 15, amount: 5, type: OrderType::SELL},
      InputOrder{userId: 3, price: 20, amount: 10, type: OrderType::SELL},
      InputOrder{userId: 2, price: 30, amount: 14, type: OrderType::SELL},
      InputOrder{userId: 4, price: 25, amount: 22, type: OrderType::BUY},
      InputOrder{userId: 5, price: 18, amount: 30, type: OrderType::SELL}};

  for (auto&& order : inputOrders)
    ob.applyOrder(order);

  EXPECT_EQ(getTotalOrdersCount(ob), 2);

  {
    auto&& orders = ob.getOrdersAtPrice(20);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(15);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(25);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(18);
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, 23);
    EXPECT_EQ(orders.front().userId, inputOrders[4].userId);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(30);
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, inputOrders[2].amount);
    EXPECT_EQ(orders.front().userId, inputOrders[2].userId);
  }
}

TEST(OrderBookBuySellMixed, CompleteBidLater2) {
  OrderBook ob;

  std::array<InputOrder, 5> inputOrders = {
      InputOrder{userId: 1, price: 15, amount: 5, type: OrderType::SELL},
      InputOrder{userId: 3, price: 20, amount: 10, type: OrderType::SELL},
      InputOrder{userId: 4, price: 25, amount: 28, type: OrderType::BUY},
      InputOrder{userId: 5, price: 18, amount: 5, type: OrderType::SELL},
      InputOrder{userId: 2, price: 23, amount: 8, type: OrderType::SELL}};

  for (auto&& order : inputOrders)
    ob.applyOrder(order);

  EXPECT_EQ(getTotalOrdersCount(ob), 0);

  {
    auto&& orders = ob.getOrdersAtPrice(20);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(15);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(25);

#if 0
    auto&& order = orders.front();
    std::cout << order << "\n";
#endif

    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(18);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(23);

#if 0
    auto&& order = orders.front();
    std::cout << order << "\n";
#endif

    EXPECT_EQ(orders.size(), 0);
  }
}

TEST(OrderBookBuySellMixed, EnoughBuyersForAsk1) {
  OrderBook ob;

  std::array<InputOrder, 4> inputOrders = {
      InputOrder{userId: 1, price: 30, amount: 5, type: OrderType::BUY},
      InputOrder{userId: 3, price: 28, amount: 10, type: OrderType::BUY},
      InputOrder{userId: 2, price: 26, amount: 8, type: OrderType::BUY},
      InputOrder{userId: 4, price: 25, amount: 25, type: OrderType::SELL}};

  for (auto&& order : inputOrders)
    ob.applyOrder(order);

  EXPECT_EQ(getTotalOrdersCount(ob), 1);

  {
    auto&& orders = ob.getOrdersAtPrice(30);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(28);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(26);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(25);
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, 2);
    EXPECT_EQ(orders.front().userId, inputOrders[3].userId);
  }
}

TEST(OrderBookBuySellMixed, EnoughBuyersForAsk2) {
  OrderBook ob;

  std::array<InputOrder, 4> inputOrders = {
      InputOrder{userId: 1, price: 30, amount: 5, type: OrderType::BUY},
      InputOrder{userId: 3, price: 27, amount: 10, type: OrderType::BUY},
      InputOrder{userId: 2, price: 20, amount: 14, type: OrderType::BUY},
      InputOrder{userId: 4, price: 25, amount: 15, type: OrderType::SELL}};

  for (auto&& order : inputOrders)
    ob.applyOrder(order);

  EXPECT_EQ(getTotalOrdersCount(ob), 1);

  {
    auto&& orders = ob.getOrdersAtPrice(30);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(27);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(25);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(20);
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, inputOrders[2].amount);
    EXPECT_EQ(orders.front().userId, inputOrders[2].userId);
  }
}

TEST(OrderBookBuySellMixed, NotEnoughBuyersForAsk1) {
  OrderBook ob;

  std::array<InputOrder, 4> inputOrders = {
      InputOrder{userId: 1, price: 28, amount: 5, type: OrderType::BUY},
      InputOrder{userId: 3, price: 26, amount: 10, type: OrderType::BUY},
      InputOrder{userId: 2, price: 20, amount: 12, type: OrderType::BUY},
      InputOrder{userId: 4, price: 25, amount: 24, type: OrderType::SELL}};

  for (auto&& order : inputOrders)
    ob.applyOrder(order);

  EXPECT_EQ(getTotalOrdersCount(ob), 2);

  {
    auto&& orders = ob.getOrdersAtPrice(28);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(26);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(25);
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, 9);
    EXPECT_EQ(orders.front().userId, inputOrders[3].userId);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(20);
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, inputOrders[2].amount);
    EXPECT_EQ(orders.front().userId, inputOrders[2].userId);
  }
}

TEST(OrderBookBuySellMixed, NotEnoughBuyersForAsk2) {
  OrderBook ob;

  std::array<InputOrder, 5> inputOrders = {
      InputOrder{userId: 1, price: 28, amount: 5, type: OrderType::BUY},
      InputOrder{userId: 3, price: 25, amount: 10, type: OrderType::BUY},
      InputOrder{userId: 2, price: 20, amount: 14, type: OrderType::BUY},
      InputOrder{userId: 4, price: 24, amount: 22, type: OrderType::SELL},
      InputOrder{userId: 5, price: 26, amount: 3, type: OrderType::BUY}};

  for (auto&& order : inputOrders)
    ob.applyOrder(order);

  EXPECT_EQ(getTotalOrdersCount(ob), 2);

  {
    auto&& orders = ob.getOrdersAtPrice(28);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(25);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(26);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(24);
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, 4);
    EXPECT_EQ(orders.front().userId, inputOrders[3].userId);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(20);
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, inputOrders[2].amount);
    EXPECT_EQ(orders.front().userId, inputOrders[2].userId);
  }
}

/**
 * @brief There is not enough buyers at first, then an appropriate match is found, and 
 * the ask is completed
 * 
 */
TEST(OrderBookBuySellMixed, CompleteAskLater1) {
  OrderBook ob;

  std::array<InputOrder, 5> inputOrders = {
      InputOrder{userId: 1, price: 25, amount: 5, type: OrderType::BUY},
      InputOrder{userId: 3, price: 27, amount: 10, type: OrderType::BUY},
      InputOrder{userId: 2, price: 20, amount: 14, type: OrderType::BUY},
      InputOrder{userId: 4, price: 24, amount: 22, type: OrderType::SELL},
      InputOrder{userId: 5, price: 28, amount: 30, type: OrderType::BUY}};

  for (auto&& order : inputOrders)
    ob.applyOrder(order);

  EXPECT_EQ(getTotalOrdersCount(ob), 2);

  {
    auto&& orders = ob.getOrdersAtPrice(25);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(27);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(24);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(28);
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, 23);
    EXPECT_EQ(orders.front().userId, inputOrders[4].userId);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(20);
    EXPECT_EQ(orders.size(), 1);

    EXPECT_EQ(orders.front().amount, inputOrders[2].amount);
    EXPECT_EQ(orders.front().userId, inputOrders[2].userId);
  }
}

TEST(OrderBookBuySellMixed, CompleteAskLater2) {
  OrderBook ob;

  std::array<InputOrder, 5> inputOrders = {
      InputOrder{userId: 1, price: 26, amount: 5, type: OrderType::BUY},
      InputOrder{userId: 3, price: 28, amount: 10, type: OrderType::BUY},
      InputOrder{userId: 4, price: 22, amount: 28, type: OrderType::SELL},
      InputOrder{userId: 5, price: 24, amount: 5, type: OrderType::BUY},
      InputOrder{userId: 2, price: 25, amount: 8, type: OrderType::BUY}};

  for (auto&& order : inputOrders)
    ob.applyOrder(order);

  EXPECT_EQ(getTotalOrdersCount(ob), 0);

  {
    auto&& orders = ob.getOrdersAtPrice(26);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(28);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(22);

#if 0
    auto&& order = orders.front();
    std::cout << order << "\n";
#endif

    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(24);
    EXPECT_EQ(orders.size(), 0);
  }

  {
    auto&& orders = ob.getOrdersAtPrice(25);

#if 0
    auto&& order = orders.front();
    std::cout << order << "\n";
#endif

    EXPECT_EQ(orders.size(), 0);
  }
}