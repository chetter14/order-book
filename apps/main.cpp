#include <chrono>
#include <iostream>
#include <thread>
#include "logger.h"
#include "order_book.h"
#include "order_generator.h"

int main() {
  std::cout << "Launched an order book app" << std::endl;

  auto logCreateResult = ob_logger::Logger::create("logs.txt");
  if (!logCreateResult.has_value()) {
    std::cout << ob_logger::to_string(logCreateResult.error()) << "\n";
    exit(1);
  }

  auto logger = std::move(logCreateResult.value());

  ob::OrderBook orderBook{};
  orderBook.setSink(logger.get());

  og::OrderGenerator orderGen{1};

  while (true) {
    auto inputOrder = orderGen.generateOrder();
    auto applyResult = orderBook.applyOrder(inputOrder);
    if (applyResult.has_value()) {
      std::cout << inputOrder << " was applied\n";
    }

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(5s);
  }

  return 0;
}