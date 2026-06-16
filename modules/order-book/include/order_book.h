#include <array>
#include <ostream>
#include <queue>

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

/**
 * @brief Prices are represented as cells in a vector. 
 * Each price has orders (or don't), that were inserted in chronological order.
 * 
 */
struct OrdersAtPrice {
  std::queue<Order> orders;
  OrderType type;
};

class OrderBook {
 public:
  void applyOrder(InputOrder&&);
  void dump(std::ostream& os);

 private:
  inline void addOrderAtPrice(const Order&, unsigned int);
  void executeBid(const Order&, unsigned int);

 private:
  /**
  * @brief Array of prices that holds bids and asks.
  * 
  */
  std::array<OrdersAtPrice, MAX_PRICE_VALUE> prices;

  /**
   * @brief Take care of top bids price and bottom asks price.
   * 
   */
  std::size_t bidsStartIdx{0}, asksStartIdx{0};
};
