#include "order_book.h"
#include <algorithm>
#include <exception>
#include <expected>
#include <iostream>
#include <ranges>

ob::InputOrder ob::buy(UserId userId, ob::Price price, Amount amount) {
  return ob::InputOrder{.userId = userId,
                        .price = price,
                        .amount = amount,
                        .type = ob::OrderType::BUY};
}

ob::InputOrder ob::sell(UserId userId, ob::Price price, Amount amount) {
  return ob::InputOrder{.userId = userId,
                        .price = price,
                        .amount = amount,
                        .type = ob::OrderType::SELL};
}

void ob::OrderBook::advanceAsksBoundary() {
  auto candidateAsks = std::views::iota(m_asksStart, MAX_PRICE_VALUE + 1);

  auto firstNotEmptyAsk = std::ranges::find_if(
      candidateAsks, [this](std::size_t i) { return !m_prices[i].empty(); });

  m_asksStart = (firstNotEmptyAsk != candidateAsks.end()) ? *firstNotEmptyAsk
                                                          : MAX_PRICE_VALUE;
}

void ob::OrderBook::retreatBidsBoundary() {
  auto candidateBids =
      std::views::iota(MIN_PRICE_VALUE, m_bidsStart + 1) | std::views::reverse;

  auto firstNotEmptyBid = std::ranges::find_if(
      candidateBids, [this](std::size_t i) { return !m_prices[i].empty(); });

  m_bidsStart = (firstNotEmptyBid != candidateBids.end()) ? *firstNotEmptyBid
                                                          : MIN_PRICE_VALUE;
}

std::ostream& ob::operator<<(std::ostream& os,
                             const ob::InputOrder& inputOrder) {
  os << "InputOrder {userId=" << inputOrder.userId
     << " price=" << inputOrder.price << " amount=" << inputOrder.amount
     << " type=" << ob::to_string(inputOrder.type) << "}";
  return os;
}

std::ostream& ob::operator<<(std::ostream& os, const ob::Order& order) {
  os << "Order {userId=" << order.userId << " amount=" << order.amount << "}";
  return os;
}

void ob::OrderBook::addOrderAtPrice(const ob::Order& order, ob::Price price) {
  m_prices[price].emplace(order.userId, order.amount);
}

/**
 * @brief Get all the orders at price. std::vector<Order> is not essentially an internal implementation. 
 * This operation is not guaranteed to be fast.
 * 
 * @param price price of orders to get 
 * @return a bunch of orders present at the price
 */
std::expected<std::vector<ob::Order>, ob::OrderBookError>
ob::OrderBook::getOrdersAtPrice(ob::Price price) const {

  if (price > MAX_PRICE_VALUE || price < MIN_PRICE_VALUE) {
    return std::unexpected(ob::OrderBookError::PRICE_OUT_OF_RANGE);
  }

  const auto& orders = m_prices[price];
  auto tempOrders = orders;

  std::vector<ob::Order> res;
  res.reserve(orders.size());

  while (!tempOrders.empty()) {
    res.push_back(tempOrders.front());
    tempOrders.pop();
  }

  return res;
}

std::size_t ob::OrderBook::getTotalOrdersCount() const {
  std::size_t count = 0U;

  std::ranges::for_each(
      m_prices, [this, &count](const auto& orders) { count += orders.size(); });

  return count;
}

void ob::OrderBook::executeOrdersAtPrice(std::queue<ob::Order>& ordersQueue,
                                         ob::Amount& sharesLeft,
                                         LogInfo&& logInfo) {
  while (!ordersQueue.empty() && sharesLeft > 0) {
    auto& order = ordersQueue.front();

    /* Bid/ask can be executed fully because there is a seller/buyer */
    if (sharesLeft >= order.amount) {
      ordersQueue.pop();
      sharesLeft -= order.amount;

      /* Amount that was executed */
      logInfo.amount = order.amount;
    }
    /* Bid/ask can be executed only partially */
    else {
      order.amount -= sharesLeft;

      logInfo.amount = sharesLeft;

      /* Buyer/seller's amount of shares is completed */
      sharesLeft = 0;
    }

    if (!m_logger) {
      if (logInfo.incomingOrderType == ob::OrderType::BUY) {
        m_logger->recordExecutedOrder({.buyer = logInfo.incomingOrderUserId,
                                       .seller = order.userId,
                                       .type = ob::OrderType::BUY,
                                       .price = logInfo.atPrice,
                                       .amount = logInfo.amount});
      } else {
        m_logger->recordExecutedOrder({.buyer = order.userId,
                                       .seller = logInfo.incomingOrderUserId,
                                       .type = ob::OrderType::SELL,
                                       .price = logInfo.atPrice,
                                       .amount = logInfo.amount});
      }
    }
  }
}

/**
 * @brief It's already assured that this bid price is higher than or equal to the lowest ask.
 * 
 * @param newOrder a bid to satisfy.
 * @param bidPrice the price that satisfies a number of asks. 
 */
void ob::OrderBook::executeBid(const ob::Order& newOrder, ob::Price bidPrice) {

  auto sharesLeft = newOrder.amount;

  /* Iterate over array to match the buy with available offers */
  for (auto i = m_asksStart; i <= bidPrice && sharesLeft > 0; ++i) {
    executeOrdersAtPrice(m_prices[i], sharesLeft,
                         {.incomingOrderType = ob::OrderType::BUY,
                          .incomingOrderUserId = newOrder.userId,
                          .atPrice = i,
                          .amount = 0});
  }

  advanceAsksBoundary();

  /* No more satisfying sellers were found for this price */
  if (sharesLeft > 0) {
    addOrderAtPrice(ob::Order{.userId = newOrder.userId, .amount = sharesLeft},
                    bidPrice);
    m_bidsStart = bidPrice;
  }
}

/**
 * @brief It's already assured that this ask price is lower than or equal to the highest bid.
 * 
 * @param newOrder an ask to satisfy.
 * @param bidPrice the price that satisfies a number of bids. 
 */
void ob::OrderBook::executeAsk(const ob::Order& newOrder, ob::Price askPrice) {

  auto sharesLeft = newOrder.amount;

  /* Iterate over array to match the ask with available buyers */
  for (auto i = m_bidsStart; i >= askPrice && sharesLeft > 0; --i) {
    executeOrdersAtPrice(m_prices[i], sharesLeft,
                         {.incomingOrderType = ob::OrderType::SELL,
                          .incomingOrderUserId = newOrder.userId,
                          .atPrice = i,
                          .amount = 0});
  }

  retreatBidsBoundary();

  /* No more satisfying buyers were found for this price */
  if (sharesLeft > 0) {
    addOrderAtPrice(ob::Order{.userId = newOrder.userId, .amount = sharesLeft},
                    askPrice);
    m_asksStart = askPrice;
  }
}

std::expected<void, ob::OrderBookError> ob::OrderBook::applyOrder(
    const ob::InputOrder& inputOrder) {

  if (inputOrder.price > MAX_PRICE_VALUE ||
      inputOrder.price < MIN_PRICE_VALUE) {
    return std::unexpected(ob::OrderBookError::PRICE_OUT_OF_RANGE);
  }

  switch (inputOrder.type) {
    case ob::OrderType::BUY: {

      auto inputOrderPrice = inputOrder.price;
      ob::Order newOrder{.userId = inputOrder.userId,
                         .amount = inputOrder.amount};

      /* Buy price covers asks price */
      if (inputOrderPrice >= m_asksStart) {
        executeBid(newOrder, inputOrderPrice);
      }
      /* Buy price exceeds the highest bid */
      else if (m_bidsStart < inputOrderPrice) {
        addOrderAtPrice(newOrder, inputOrderPrice);
        m_bidsStart = inputOrderPrice;
      }
      /* Buy price is lower than or equal to the highest bid */
      else {
        addOrderAtPrice(newOrder, inputOrderPrice);
      }
      break;
    }

    case ob::OrderType::SELL: {
      auto inputOrderPrice = inputOrder.price;
      ob::Order newOrder{.userId = inputOrder.userId,
                         .amount = inputOrder.amount};

      /* Sell price covers bids price */
      if (inputOrderPrice <= m_bidsStart) {
        executeAsk(newOrder, inputOrderPrice);
      }
      /* Sell price is below the lowest ask */
      else if (inputOrderPrice < m_asksStart) {
        addOrderAtPrice(newOrder, inputOrderPrice);
        m_asksStart = inputOrderPrice;
      }
      /* Sell price is higher than or equal to the lowest ask */
      else {
        addOrderAtPrice(newOrder, inputOrderPrice);
      }
      break;
    }
  }

  return {};
}

void ob::OrderBook::dump(std::ostream& os) const {
  /* TO-DO: implement */
}