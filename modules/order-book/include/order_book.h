#include <array>
#include <limits>
#include <ostream>
#include <queue>
#include <gtest/gtest_prod.h>

constexpr std::size_t MAX_PRICE_VALUE = 10000;

enum class OrderType { BUY, SELL, UNDEFINED };

/**
 * @brief Orders coming from the input source.
 * 
 */
struct InputOrder {
  unsigned long long userId;
  unsigned int price;
  unsigned int amount;
  OrderType type;
};

/**
 * @brief Orders handled inside the order book. Price of order is known.
 * 
 */
struct Order {
  unsigned long long userId;
  unsigned int amount;
};

std::ostream& operator<<(std::ostream& os, const Order& order);

class OrderBook {
 public:
  void applyOrder(const InputOrder&);

  void dump(std::ostream& os) const;

 private:
  inline void addOrderAtPrice(const Order&, unsigned int);
  inline const std::queue<Order>& getOrdersAtPrice(unsigned int) const;
  
  std::size_t getTotalOrdersCount(const OrderBook& ob) const;

  void executeBid(const Order&, unsigned int);
  void executeAsk(const Order&, unsigned int);

  void advanceAsksBoundary();
  void retreatBidsBoundary();

  FRIEND_TEST(OrderBookTestSuite);

 private:
  /**
  * @brief Array of prices that holds bids and asks.
  * 
  */
  std::array<std::queue<Order>, MAX_PRICE_VALUE + 1> prices;

  /**
   * @brief Take care of top bids price and bottom asks price.
   * 
   */
  std::size_t bidsStartIdx{0}, asksStartIdx{MAX_PRICE_VALUE};
};
