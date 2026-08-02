#ifndef ORDER_BOOK_CUSTOM_TYPES_H
#define ORDER_BOOK_CUSTOM_TYPES_H

#include <string_view>

namespace ob {

using UserId = unsigned long long;
using Price = unsigned int;
using Amount = unsigned int;

enum class OrderType { BUY, SELL, UNDEFINED };

constexpr std::string_view to_string(OrderType orderType) {
  switch (orderType) {
    case OrderType::BUY:
      return "BUY";
    case OrderType::SELL:
      return "SELL";
    case OrderType::UNDEFINED:
      return "UNDEFINED";
  }
}

/**
 * @brief Executed orders going into the logger module.
 * 
 */
struct ExecutedOrder {
  UserId buyer;
  UserId seller;
  OrderType type;
  Price price;
  Amount amount;
};

/**
 * @brief Orders coming from the input source.
 * 
 */
struct InputOrder {
  UserId userId;
  Price price;
  Amount amount;
  OrderType type;
};

}  // namespace ob

#endif  // ORDER_BOOK_CUSTOM_TYPES_H