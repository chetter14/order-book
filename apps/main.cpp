#include <chrono>
#include <iostream>
#include <thread>
#include "logger.h"
#include "order_book.h"
#include "order_generator.h"

int main() {
  std::cout << "Launched an order book app" << std::endl;

  auto result = ob_logger::Logger::create("logs.txt");
  if (!result.has_value()) {
    std::cout << ob_logger::to_string(result.error()) << "\n";
    exit(1);
  }

  ob::OrderBook orderBook{};
  orderBook.setLogger(std::move(result.value()));

  og::OrderGenerator orderGen{1};

  while (true) {
    auto inputOrder = orderGen.generateOrder();
    auto result = orderBook.applyOrder(inputOrder);
    if (result.has_value()) {
      std::cout << inputOrder << " was applied\n";
    }

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(5s);
  }

  return 0;
}