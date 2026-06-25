#include <array>
#include <expected>
#include <limits>
#include <ostream>
#include <queue>
#include <vector>

constexpr std::size_t MAX_PRICE_VALUE = 9999U, MIN_PRICE_VALUE = 1U;

enum class OrderType { BUY, SELL, UNDEFINED };

using Price = unsigned int;

/**
 * @brief Orders coming from the input source.
 * 
 */
struct InputOrder {
  unsigned long long userId;
  Price price;
  unsigned int amount;
  OrderType type;
};

InputOrder buy(unsigned long long userId, Price price, unsigned int amount);

InputOrder sell(unsigned long long userId, Price price, unsigned int amount);

/**
 * @brief Orders handled inside the order book. Price of order is known.
 * 
 */
struct Order {
  unsigned long long userId;
  unsigned int amount;
};

enum class OrderBookError { PRICE_OUT_OF_RANGE };

std::ostream& operator<<(std::ostream& os, const Order& order);

class OrderBook {
 public:
  std::expected<void, OrderBookError> applyOrder(const InputOrder&);

  std::size_t getTotalOrdersCount() const;
  std::expected<std::vector<Order>, OrderBookError> getOrdersAtPrice(
      Price) const;

  void dump(std::ostream& os) const;

 private:
  void addOrderAtPrice(const Order&, Price);

  void executeBid(const Order&, Price);
  void executeAsk(const Order&, Price);

  void advanceAsksBoundary();
  void retreatBidsBoundary();

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
  Price bidsStart{MIN_PRICE_VALUE}, asksStart{MAX_PRICE_VALUE};
};
