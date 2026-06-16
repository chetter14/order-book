#include "order_book.h"

void OrderBook::addOrderAtPrice(Order&& order, unsigned int price) {
  this->prices[price].orders.emplace(std::move(order));
}

void OrderBook::applyOrder(InputOrder&& inputOrder) {
  switch (inputOrder.type) {
    case OrderType::BUY: {
        
      auto inputOrderPrice = inputOrder.price;
      Order newOrder{userId: inputOrder.userId, amount: inputOrder.amount};

      /* Buy price exceeds the top one */
      if (this->bidsStartIdx < inputOrderPrice) {
        addOrderAtPrice(std::move(newOrder), inputOrderPrice);
        bidsStartIdx = inputOrderPrice;
      }
      /* Buy price is lower than the top one */
      else if (this->bidsStartIdx > inputOrderPrice) {
        addOrderAtPrice(std::move(newOrder), inputOrderPrice);
      }
      break;
    }

    case OrderType::SELL: {
      break;
    }
  }
}

void OrderBook::dump(std::ostream& os) {}