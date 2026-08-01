#ifndef ORDER_BOOK_LOGGER_H
#define ORDER_BOOK_LOGGER_H

#include <chrono>
#include <expected>
#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include "order_book.h"

namespace ob_logger {

std::string formatExecutedOrder(const ob::ExecutedOrder& order,
                                std::chrono::system_clock::time_point when);

enum class LoggerError { FAILED_TO_OPEN_FILE };

constexpr std::string_view to_string(LoggerError loggerError) {
  switch (loggerError) {
    case LoggerError::FAILED_TO_OPEN_FILE:
      return "FAILED_TO_OPEN_FILE";
  }
}

class Logger {
 private:
  Logger() = default;

 public:
  static std::expected<std::unique_ptr<Logger>, LoggerError> create(
      const std::filesystem::path& path);

  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;

  Logger(Logger&&) = delete;
  Logger& operator=(Logger&&) = delete;

  ~Logger() = default;

  void recordExecutedOrder(const ob::ExecutedOrder& order);

 private:
  std::ofstream out_;
  std::mutex mtx_;
};

}  // namespace ob_logger

#endif
