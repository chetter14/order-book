#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

#include <array>
#include <expected>
#include <limits>
#include <memory>
#include <ostream>
#include <queue>
#include <string>
#include <vector>
#include "logger.h"

namespace ob {

using UserId = unsigned long long;
using Price = unsigned int;
using Amount = unsigned int;

constexpr Price MAX_PRICE_VALUE = 9999U, MIN_PRICE_VALUE = 1U;

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
 * @brief Orders coming from the input source.
 * 
 */
struct InputOrder {
  UserId userId;
  Price price;
  Amount amount;
  OrderType type;
};

std::ostream& operator<<(std::ostream& os, const InputOrder& order);

InputOrder buy(UserId userId, Price price, Amount amount);
InputOrder sell(UserId userId, Price price, Amount amount);

/**
 * @brief Orders handled inside the order book. Price of order is known.
 * 
 */
struct Order {
  UserId userId;
  Amount amount;
};

enum class OrderBookError { PRICE_OUT_OF_RANGE };

std::ostream& operator<<(std::ostream& os, const Order& order);

struct ExecutedOrder {
  UserId buyer;
  UserId seller;
  OrderType type;
  Price price;
  Amount amount;
};

class OrderBook {
 public:
  std::expected<void, OrderBookError> applyOrder(const InputOrder&);

  std::size_t getTotalOrdersCount() const;
  std::expected<std::vector<Order>, OrderBookError> getOrdersAtPrice(
      Price) const;

  void setLogger(std::unique_ptr<ob_logger::Logger>&& logger) {
    m_logger = std::move(logger);
  }

  void dump(std::ostream& os) const;

 private:
  struct LogInfo {
    ob::OrderType incomingOrderType;
    ob::UserId incomingOrderUserId;
    ob::Price atPrice;
    ob::Amount amount;
  };

 private:
  void addOrderAtPrice(const Order&, Price);

  void executeBid(const Order&, Price);
  void executeAsk(const Order&, Price);

  void executeOrdersAtPrice(std::queue<ob::Order>& ordersQueue,
                            ob::Amount& sharesLeft, LogInfo&& logInfo);

  void advanceAsksBoundary();
  void retreatBidsBoundary();

 private:
  /**
  * @brief Array of prices that holds bids and asks.
  * 
  */
  std::array<std::queue<Order>, MAX_PRICE_VALUE + 1> m_prices;

  /**
   * @brief Take care of top bids price and bottom asks price.
   * 
   */
  Price m_bidsStart{MIN_PRICE_VALUE}, m_asksStart{MAX_PRICE_VALUE};

  std::unique_ptr<ob_logger::Logger> m_logger;
};

}  // namespace ob

#endif