#include "order_book.h"

std::ostream& operator<<(std::ostream& os, const Order& order) {
  os << "Order {userId=" << order.userId << " amount=" << order.amount << "}";
  return os;
}

void OrderBook::addOrderAtPrice(const Order& order, unsigned int price) {
  this->prices[price].emplace(order.userId, order.amount);
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
    auto& ordersQueue = this->prices[i];
    auto queueSize = ordersQueue.size();

    for (std::size_t j = 0; j < queueSize && sharesLeft > 0; ++j) {
      auto& sellOrder = ordersQueue.front();

      /* Ask can be executed fully because there is a buyer */
      if (sharesLeft >= sellOrder.amount) {
        ordersQueue.pop();
        sharesLeft -= sellOrder.amount;
      }
      /* Ask can be executed only partially */
      else {
        sellOrder.amount -= sharesLeft;
        /* Buyer's amount of shares is completed */
        sharesLeft = 0;
      }
    }
  }

  /* No more satisfying sellers were found for this price */
  if (sharesLeft > 0) {
    this->prices[bidPrice].emplace(newOrder.userId, sharesLeft);
    this->bidsStartIdx = bidPrice;
  }
}

void OrderBook::applyOrder(const InputOrder& inputOrder) {
  switch (inputOrder.type) {
    case OrderType::BUY: {

      auto inputOrderPrice = inputOrder.price;
      Order newOrder{userId: inputOrder.userId, amount: inputOrder.amount};

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
      Order newOrder{userId: inputOrder.userId, amount: inputOrder.amount};

      /* Sell price covers bids price */
      if (inputOrderPrice <= this->bidsStartIdx) {
        executeAsk(newOrder, inputOrderPrice);
      }
      /* Sell price is below the lowest ask */
      else if (inputOrderPrice < this->asksStartIdx) {
        addOrderAtPrice(newOrder, inputOrderPrice);
        this->bidsStartIdx = inputOrderPrice;
      }
      /* Sell price is higher than or equal to the lowest ask */
      else {
        addOrderAtPrice(newOrder, inputOrderPrice);
      }
      break;
    }
  }
}

const std::queue<Order>& OrderBook::getOrdersAtPrice(unsigned int price) const {
  return this->prices[price];
}

void OrderBook::dump(std::ostream& os) const {}