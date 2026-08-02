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
#include "custom_types.h"
#include "execution_sink.h"

namespace ob {

constexpr Price MAX_PRICE_VALUE = 9999U, MIN_PRICE_VALUE = 1U;

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

class OrderBook {
 public:
  std::expected<void, OrderBookError> applyOrder(const InputOrder&);

  std::size_t getTotalOrdersCount() const;
  std::expected<std::vector<Order>, OrderBookError> getOrdersAtPrice(
      Price) const;

  void setSink(ExecutionSink *sink) {
    m_sink = sink;
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

  ExecutionSink *m_sink = nullptr;
};

}  // namespace ob

#endif