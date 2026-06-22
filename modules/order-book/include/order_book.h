#include <array>
#include <limits>
#include <ostream>
#include <queue>
#include <vector>

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

InputOrder buy(unsigned long long userId, unsigned int price,
               unsigned int amount);

InputOrder sell(unsigned long long userId, unsigned int price,
                unsigned int amount);

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

  std::size_t getTotalOrdersCount() const;
  std::vector<Order> getOrdersAtPrice(unsigned int) const;

  void dump(std::ostream& os) const;

 private:
  void addOrderAtPrice(const Order&, unsigned int);

  void executeBid(const Order&, unsigned int);
  void executeAsk(const Order&, unsigned int);

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
  std::size_t bidsStartIdx{0}, asksStartIdx{MAX_PRICE_VALUE};
};
