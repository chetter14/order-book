#include "order_generator.h"
#include <cmath>

ob::InputOrder og::OrderGenerator::generateOrder() {
  return ob::InputOrder{.userId = nextUserId(),
                        .price = nextPrice(),
                        .amount = nextAmount(),
                        .type = nextOrderType()};
}

ob::UserId og::OrderGenerator::nextUserId() {
  return userIdGen(generatorEngine);
}

ob::Price og::OrderGenerator::nextPrice() {
  for (;;) {
    const auto v = std::lround(priceGen(generatorEngine));
    if (v >= static_cast<long>(ob::MIN_PRICE_VALUE) &&
        v <= static_cast<long>(ob::MAX_PRICE_VALUE)) {
      return static_cast<ob::Price>(v);
    }
  }
}

ob::Amount og::OrderGenerator::nextAmount() {
  return amountGen(generatorEngine);
}

ob::OrderType og::OrderGenerator::nextOrderType() {
  return orderTypeGen(generatorEngine) ? ob::OrderType::BUY
                                       : ob::OrderType::SELL;
}
