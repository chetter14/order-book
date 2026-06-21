#include "order_book.h"

void OrderBook::advanceAsksBoundary() {
  auto curAsksIdx = this->asksStartIdx;
  while (this->prices[curAsksIdx].empty() && curAsksIdx < MAX_PRICE_VALUE) {
    curAsksIdx++;
  }
  this->asksStartIdx = curAsksIdx;
}

void OrderBook::retreatBidsBoundary() {
  auto curBidsIdx = this->bidsStartIdx;
  while (this->prices[curBidsIdx].empty() && curBidsIdx > 0) {
    curBidsIdx--;
  }
  this->bidsStartIdx = curBidsIdx;
}

std::ostream& operator<<(std::ostream& os, const Order& order) {
  os << "Order {userId=" << order.userId << " amount=" << order.amount << "}";
  return os;
}

void OrderBook::addOrderAtPrice(const Order& order, unsigned int price) {
  this->prices[price].emplace(order.userId, order.amount);
}

const std::queue<Order>& OrderBook::getOrdersAtPrice(unsigned int price) const {
  return this->prices[price];
}

std::size_t OrderBook::getTotalOrdersCount(const OrderBook& ob) const {
  std::size_t count = 0U;
  for (auto i = 0U; i < MAX_PRICE_VALUE; ++i) {
    count += ob.getOrdersAtPrice(i).size();
  }
  return count;
}

void executeOrdersAtPrice(std::queue<Order>& ordersQueue,
                          unsigned int& sharesLeft) {
  auto queueSize = ordersQueue.size();

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
void OrderBook::executeBid(const Order& newOrder, unsigned int bidPrice) {

  auto sharesLeft = newOrder.amount;

  /* Iterate over array to match the buy with available offers */
  for (std::size_t i = this->asksStartIdx; i <= bidPrice && sharesLeft > 0;
       ++i) {
    executeOrdersAtPrice(this->prices[i], sharesLeft);

    if (this->prices[i].empty()) {
      this->asksStartIdx = i + 1;
    }
  }

  advanceAsksBoundary();

  /* No more satisfying sellers were found for this price */
  if (sharesLeft > 0) {
    this->prices[bidPrice].emplace(newOrder.userId, sharesLeft);
    this->bidsStartIdx = bidPrice;
  }
}

/**
 * @brief It's already assured that this ask price is lower than the highest bid.
 * 
 * @param newOrder an ask to satisfy.
 * @param bidPrice the price that satisfies a number of bids. 
 */
void OrderBook::executeAsk(const Order& newOrder, unsigned int askPrice) {

  auto sharesLeft = newOrder.amount;

  /* Iterate over array to match the ask with available buyers */
  for (std::size_t i = this->bidsStartIdx; i >= askPrice && sharesLeft > 0;
       --i) {
    executeOrdersAtPrice(this->prices[i], sharesLeft);

    if (this->prices[i].empty()) {
      this->bidsStartIdx = i - 1;
    }
  }

  retreatBidsBoundary();

  /* No more satisfying buyers were found for this price */
  if (sharesLeft > 0) {
    this->prices[askPrice].emplace(newOrder.userId, sharesLeft);
    this->asksStartIdx = askPrice;
  }
}

void OrderBook::applyOrder(const InputOrder& inputOrder) {
  switch (inputOrder.type) {
    case OrderType::BUY: {

      auto inputOrderPrice = inputOrder.price;
      Order newOrder{.userId = inputOrder.userId, .amount = inputOrder.amount};

      /* Buy price covers asks price */
      if (inputOrderPrice >= this->asksStartIdx) {
        executeBid(newOrder, inputOrderPrice);
      }
      /* Buy price exceeds the highest bid */
      else if (this->bidsStartIdx < inputOrderPrice) {
        addOrderAtPrice(newOrder, inputOrderPrice);
        this->bidsStartIdx = inputOrderPrice;
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
      if (inputOrderPrice <= this->bidsStartIdx) {
        executeAsk(newOrder, inputOrderPrice);
      }
      /* Sell price is below the lowest ask */
      else if (inputOrderPrice < this->asksStartIdx) {
        addOrderAtPrice(newOrder, inputOrderPrice);
        this->asksStartIdx = inputOrderPrice;
      }
      /* Sell price is higher than or equal to the lowest ask */
      else {
        addOrderAtPrice(newOrder, inputOrderPrice);
      }
      break;
    }
  }
}

void OrderBook::dump(std::ostream& os) const {}