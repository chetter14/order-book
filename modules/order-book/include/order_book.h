#include <ostream>
#include <queue>
#include <vector>

enum class OrderType { BUY, SELL };

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
struct PriceOrders {
  std::queue<Order> orders;
  OrderType type;
};

class OrderBook {
 public:
  OrderBook() {}
  void applyOrder(InputOrder&&);

  void dump(std::ostream& os);

 private:
  std::vector<PriceOrders> prices;
};
