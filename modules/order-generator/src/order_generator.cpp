#include "order_generator.h"
#include <cmath>

ob::InputOrder og::OrderGenerator::generateOrder() {
  return ob::InputOrder{.userId = nextUserId(),
                        .price = nextPrice(),
                        .amount = nextAmount(),
                        .type = nextOrderType()};
}

ob::UserId og::OrderGenerator::nextUserId() {
  return userIdGen_(generatorEngine_);
}

ob::Price og::OrderGenerator::nextPrice() {
  for (;;) {
    const auto v = std::lround(priceGen_(generatorEngine_));
    if (v >= static_cast<long>(ob::MIN_PRICE_VALUE) &&
        v <= static_cast<long>(ob::MAX_PRICE_VALUE)) {
      return static_cast<ob::Price>(v);
    }
  }
}

ob::Amount og::OrderGenerator::nextAmount() {
  return amountGen_(generatorEngine_);
}

ob::OrderType og::OrderGenerator::nextOrderType() {
  return orderTypeGen_(generatorEngine_) ? ob::OrderType::BUY
                                       : ob::OrderType::SELL;
}
