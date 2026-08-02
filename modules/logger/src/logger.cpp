#include "logger.h"
#include <sstream>

std::expected<std::unique_ptr<ob_logger::Logger>, ob_logger::LoggerError>
ob_logger::Logger::create(const std::filesystem::path& path) {

  std::unique_ptr<ob_logger::Logger> logger{new Logger()};

  logger->m_out = std::ofstream{path, std::ios::app};
  if (!logger->m_out) {
    return std::unexpected(ob_logger::LoggerError::FAILED_TO_OPEN_FILE);
  }

  return logger;
};

std::string ob_logger::formatExecutedOrder(
    const ob::ExecutedOrder& order,
    std::chrono::system_clock::time_point when) {
  std::ostringstream os;

  /* "[{} UTC] OPERATION={}, BUYER={}, SELLER={}, PRICE={}, AMOUNT={}" */
  os << "[" << std::chrono::floor<std::chrono::milliseconds>(when)
     << " UTC] OPERATION=" << ob::to_string(order.type)
     << ", BUYER=" << order.buyer << ", SELLER=" << order.seller
     << ", PRICE=" << order.price << ", AMOUNT=" << order.amount;

  return os.str();
}

void ob_logger::Logger::recordExecutedOrder(const ob::ExecutedOrder& order) {

  std::lock_guard lock{m_mtx};

  const auto now = std::chrono::system_clock::now();

  m_out << formatExecutedOrder(order, now) << "\n";
  m_out.flush();
}