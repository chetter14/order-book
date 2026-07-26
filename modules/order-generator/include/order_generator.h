#ifndef ORDER_GENERATOR_H
#define ORDER_GENERATOR_H

#include <random>
#include "order_book.h"

namespace og {

constexpr ob::Amount MIN_AMOUNT_VALUE = 1U, MAX_AMOUNT_VALUE = 1000U;
constexpr ob::UserId MIN_USER_ID = 1U, MAX_USER_ID = 1000U;

constexpr double PRICE_STDDEV = 1250;

class OrderGenerator {
 public:
  explicit OrderGenerator(unsigned int seed)
      : generatorEngine(seed),
        userIdGen(MIN_USER_ID, MAX_USER_ID),
        priceGen(ob::MIN_PRICE_VALUE +
                     (ob::MAX_PRICE_VALUE - ob::MIN_PRICE_VALUE) / 2,
                 PRICE_STDDEV),
        amountGen(MIN_AMOUNT_VALUE, MAX_AMOUNT_VALUE),
        orderTypeGen(0.5) {}

  /**
  * @brief Generates an input order with random values:
  * 
  * userId - uniform distribution in [MIN_USER_ID, MAX_USER_ID].
  * 
  * price - normal distribution in [MIN_PRICE_VALUE, MAX_PRICE_VALUE].
  * The range is fixed, values outside of it are truncated to MIN/MAX.
  * 
  * amount - uniform distribution in [MIN_AMOUNT_VALUE, MAX_AMOUNT_VALUE].
  * 
  * type - buy/sell is 50/50.
  * 
  * @return ob::InputOrder 
  */
  ob::InputOrder generateOrder();

  /* Methods for generating random values for input orders */
 private:
  ob::UserId nextUserId();
  ob::Price nextPrice();
  ob::Amount nextAmount();
  ob::OrderType nextOrderType();

  /* Variables that are used for generating random values */
 private:
  std::mt19937 generatorEngine;

  std::uniform_int_distribution<ob::UserId> userIdGen;
  std::normal_distribution<double> priceGen;
  std::uniform_int_distribution<ob::Amount> amountGen;
  std::bernoulli_distribution orderTypeGen;
};
}  // namespace og

#endif
