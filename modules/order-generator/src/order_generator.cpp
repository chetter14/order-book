#include "order_generator.h"
#include <random>

namespace {
constexpr std::size_t MIN_AMOUNT_VALUE = 1U, MAX_AMOUNT_VALUE = 1000U;
constexpr std::size_t MIN_USER_ID = 1U, MAX_USER_ID = 1000U;

constexpr double stddev = 1250;

std::mt19937 generatorEngine;
std::uniform_int_distribution<unsigned int> amountGen(MIN_AMOUNT_VALUE,
                                                      MAX_AMOUNT_VALUE);
std::normal_distribution<double> priceGen(
    (ob::MAX_PRICE_VALUE - ob::MIN_PRICE_VALUE) / 2, stddev);

unsigned long long curUserId;
ob::OrderType curOrderType;

inline unsigned long long nextUserId() {
  if (curUserId > MAX_USER_ID)
    curUserId = MIN_USER_ID;

  return curUserId++;
}

inline ob::OrderType nextOrderType() {
  switch (curOrderType) {
    case ob::OrderType::BUY:
      curOrderType = ob::OrderType::SELL;
      break;
    case ob::OrderType::SELL:
      curOrderType = ob::OrderType::BUY;
      break;
    case ob::OrderType::UNDEFINED:
      curOrderType = ob::OrderType::UNDEFINED;
      break;
  }

  return curOrderType;
}

}  // namespace

void order_generator::initGenerator(unsigned int seed) {
  generatorEngine.seed(seed);

  curUserId = MIN_USER_ID;
  curOrderType = ob::OrderType::BUY;
}

/**
 * @brief Generates an input order with these values:
 * 
 * userId - sequentially and round up in the end (for example: 1, 2, 3, ..., 100, 1, 2, ...).
 * 
 * price - normal distribution between MIN_PRICE_VALUE and MAX_PRICE_VALUE.
 * 
 * amount - even distribution from MIN_AMOUNT_VALUE and MAX_AMOUNT_VALUE.
 * 
 * type - alternate between BUY and SELL (BUY, SELL, BUY, SELL, ...).
 * 
 * @return ob::InputOrder 
 */
ob::InputOrder order_generator::generateOrder() {
  return ob::InputOrder{
      .userId = nextUserId(),
      .price = static_cast<ob::Price>(std::lround(priceGen(generatorEngine))),
      .amount = amountGen(generatorEngine),
      .type = nextOrderType()};
}