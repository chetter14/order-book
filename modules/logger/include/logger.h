#ifndef ORDER_BOOK_LOGGER_H
#define ORDER_BOOK_LOGGER_H

#include "order_book.h"

namespace ob_logger {

struct ExecutedOrder {
  ob::UserId buyer;
  ob::UserId seller;
  ob::OrderType type;
  ob::Price price;
  ob::Amount amount;
};

class Logger {
 public:
  Logger();

  void recordExecutedOrder(const ExecutedOrder& order);
};

}  // namespace ob_logger

#endif
