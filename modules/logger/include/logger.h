#ifndef ORDER_BOOK_LOGGER_H
#define ORDER_BOOK_LOGGER_H

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <print>
#include "order_book.h"

namespace ob_logger {

/* Not used now; for future extensions */
enum class LogLevel { INFO, WARNING, ERROR };

struct ExecutedOrder {
  ob::UserId buyer;
  ob::UserId seller;
  ob::OrderType type;
  ob::Price price;
  ob::Amount amount;
};

class Logger {
 public:
  explicit Logger(const std::filesystem::path& path)
      : out_{path, std::ios::app} {
    if (!out_) {
      std::cerr << "Failed to open the file - " << path.string() << " !"
                << std::endl;
      exit(1);
    }
  };

  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;

  void recordExecutedOrder(const ExecutedOrder& order) {
    const auto now = std::chrono::floor<std::chrono::milliseconds>(
        std::chrono::system_clock::now());

    const std::chrono::zoned_time timestamp{std::chrono::current_zone(), now};

    std::lock_guard lock{mtx_};
    std::println(out_,
                 "[{}] OPERATION={}, BUYER={}, SELLER={}, PRICE={}, AMOUNT={}",
                 timestamp, ob::to_string(order.type), order.buyer,
                 order.seller, order.price, order.amount);
    out_.flush();
  }

 private:
  std::ofstream out_;
  std::mutex mtx_;
};

}  // namespace ob_logger

#endif
