#include "order_book.h"
#include <algorithm>
#include <exception>
#include <expected>
#include <iostream>
#include <ranges>

InputOrder buy(unsigned long long userId, Price price, unsigned int amount) {
  return InputOrder{.userId = userId,
                    .price = price,
                    .amount = amount,
                    .type = OrderType::BUY};
}

InputOrder sell(unsigned long long userId, Price price, unsigned int amount) {
  return InputOrder{.userId = userId,
                    .price = price,
                    .amount = amount,
                    .type = OrderType::SELL};
}

void OrderBook::advanceAsksBoundary() {
  auto candidateAsks = std::views::iota(asksStart, MAX_PRICE_VALUE + 1);

  auto firstNotEmptyAsk = std::ranges::find_if(
      candidateAsks,
      [this](std::size_t i) { return !this->prices[i].empty(); });

  this->asksStart = (firstNotEmptyAsk != candidateAsks.end())
                        ? *firstNotEmptyAsk
                        : MAX_PRICE_VALUE;
}

void OrderBook::retreatBidsBoundary() {
  auto candidateBids =
      std::views::iota(MIN_PRICE_VALUE, bidsStart + 1) | std::views::reverse;

  auto firstNotEmptyBid = std::ranges::find_if(
      candidateBids,
      [this](std::size_t i) { return !this->prices[i].empty(); });

  this->bidsStart = (firstNotEmptyBid != candidateBids.end())
                        ? *firstNotEmptyBid
                        : MIN_PRICE_VALUE;
}

std::ostream& operator<<(std::ostream& os, const Order& order) {
  os << "Order {userId=" << order.userId << " amount=" << order.amount << "}";
  return os;
}

void OrderBook::addOrderAtPrice(const Order& order, Price price) {
  this->prices[price].emplace(order.userId, order.amount);
}

/**
 * @brief Get all the orders at price. std::vector<Order> is not essentially an internal implementation. 
 * This operation is not guaranteed to be fast.
 * 
 * @param price price of orders to get 
 * @return a bunch of orders present at the price
 */
std::expected<std::vector<Order>, OrderBookError> OrderBook::getOrdersAtPrice(
    Price price) const {

  if (price > MAX_PRICE_VALUE || price < MIN_PRICE_VALUE) {
    return std::unexpected(OrderBookError::PRICE_OUT_OF_RANGE);
  }

  const auto& orders = this->prices[price];
  auto tempOrders = orders;

  std::vector<Order> res;
  res.reserve(orders.size());

  while (!tempOrders.empty()) {
    res.push_back(tempOrders.front());
    tempOrders.pop();
  }

  return res;
}

std::size_t OrderBook::getTotalOrdersCount() const {
  std::size_t count = 0U;

  std::ranges::for_each(this->prices, [this, &count](const auto& orders) {
    count += orders.size();
  });

  return count;
}

void executeOrdersAtPrice(std::queue<Order>& ordersQueue,
                          unsigned int& sharesLeft) {
  while (!ordersQueue.empty() && sharesLeft > 0) {
    auto& order = ordersQueue.front();

    /* Bid/ask can be executed fully because there is a seller/buyer */
    if (sharesLeft >= order.amount) {
      ordersQueue.pop();
      sharesLeft -= order.amount;
    }
    /* Bid/ask can be executed only partially */
    else {
      order.amount -= sharesLeft;
      /* Buyer/seller's amount of shares is completed */
      sharesLeft = 0;
    }
  }
}

/**
 * @brief It's already assured that this bid price is higher than the lowest ask.
 * 
 * @param newOrder a bid to satisfy.
 * @param bidPrice the price that satisfies a number of asks. 
 */
void OrderBook::executeBid(const Order& newOrder, Price bidPrice) {

  auto sharesLeft = newOrder.amount;

  /* Iterate over array to match the buy with available offers */
  for (auto i = this->asksStart; i <= bidPrice && sharesLeft > 0; ++i) {
    executeOrdersAtPrice(this->prices[i], sharesLeft);
  }

  advanceAsksBoundary();

  /* No more satisfying sellers were found for this price */
  if (sharesLeft > 0) {
    addOrderAtPrice(Order{.userId = newOrder.userId, .amount = sharesLeft},
                    bidPrice);
    this->bidsStart = bidPrice;
  }
}

/**
 * @brief It's already assured that this ask price is lower than the highest bid.
 * 
 * @param newOrder an ask to satisfy.
 * @param bidPrice the price that satisfies a number of bids. 
 */
void OrderBook::executeAsk(const Order& newOrder, Price askPrice) {

  auto sharesLeft = newOrder.amount;

  /* Iterate over array to match the ask with available buyers */
  for (auto i = this->bidsStart; i >= askPrice && sharesLeft > 0; --i) {
    executeOrdersAtPrice(this->prices[i], sharesLeft);
  }

  retreatBidsBoundary();

  /* No more satisfying buyers were found for this price */
  if (sharesLeft > 0) {
    addOrderAtPrice(Order{.userId = newOrder.userId, .amount = sharesLeft},
                    askPrice);
    this->asksStart = askPrice;
  }
}

std::expected<void, OrderBookError> OrderBook::applyOrder(
    const InputOrder& inputOrder) {

  if (inputOrder.price > MAX_PRICE_VALUE ||
      inputOrder.price < MIN_PRICE_VALUE) {
    return std::unexpected(OrderBookError::PRICE_OUT_OF_RANGE);
  }

  switch (inputOrder.type) {
    case OrderType::BUY: {

      auto inputOrderPrice = inputOrder.price;
      Order newOrder{.userId = inputOrder.userId, .amount = inputOrder.amount};

      /* Buy price covers asks price */
      if (inputOrderPrice >= this->asksStart) {
        executeBid(newOrder, inputOrderPrice);
      }
      /* Buy price exceeds the highest bid */
      else if (this->bidsStart < inputOrderPrice) {
        addOrderAtPrice(newOrder, inputOrderPrice);
        this->bidsStart = inputOrderPrice;
      }
      /* Buy price is lower than or equal to the highest bid */
      else {
        addOrderAtPrice(newOrder, inputOrderPrice);
      }
      break;
    }

    case OrderType::SELL: {
      auto inputOrderPrice = inputOrder.price;
      Order newOrder{.userId = inputOrder.userId, .amount = inputOrder.amount};

      /* Sell price covers bids price */
      if (inputOrderPrice <= this->bidsStart) {
        executeAsk(newOrder, inputOrderPrice);
      }
      /* Sell price is below the lowest ask */
      else if (inputOrderPrice < this->asksStart) {
        addOrderAtPrice(newOrder, inputOrderPrice);
        this->asksStart = inputOrderPrice;
      }
      /* Sell price is higher than or equal to the lowest ask */
      else {
        addOrderAtPrice(newOrder, inputOrderPrice);
      }
      break;
    }
  }

  return {};
}

void OrderBook::dump(std::ostream& os) const {
  /* TO-DO: implement */
}