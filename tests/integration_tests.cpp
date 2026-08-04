#include <gtest/gtest.h>
#include <charconv>
#include <filesystem>
#include <fstream>
#include <regex>
#include <string>
#include <vector>
#include "execution_sink.h"
#include "logger.h"
#include "order_book.h"
#include "order_generator.h"

namespace {
class RecordingSink : public ob::ExecutionSink {
 public:
  void onExecuted(const ob::ExecutedOrder& eo) override {
    executions.push_back(eo);
  }
  std::vector<ob::ExecutedOrder> executions;
};
}  // namespace

TEST(Pipeline, SharesAreConserved) {
  og::OrderGenerator orderGen{1};
  ob::OrderBook orderBook{};
  RecordingSink sink;
  orderBook.setSink(&sink);

  std::size_t submitted = 0U;
  for (auto i = 0U; i < 1000000; ++i) {
    auto inputOrder = orderGen.generateOrder();
    if (orderBook.applyOrder(inputOrder).has_value()) {
      submitted += inputOrder.amount;
    }
  }

  std::size_t resting = 0U;
  for (auto price = ob::MIN_PRICE_VALUE; price <= ob::MAX_PRICE_VALUE;
       ++price) {
    for (const auto& order : orderBook.getOrdersAtPrice(price).value()) {
      resting += order.amount;
    }
  }

  std::size_t executed = 0U;
  for (const auto& executedOrder : sink.executions) {
    executed += executedOrder.amount;
  }

  EXPECT_EQ(submitted, resting + 2 * executed);
}

TEST(Pipeline, TradeExecutesAtRestingPrice) {
  ob::OrderBook orderBook{};
  RecordingSink sink;
  orderBook.setSink(&sink);

  orderBook.applyOrder(ob::sell(1, 100, 20));
  orderBook.applyOrder(ob::buy(2, 120, 20));

  ASSERT_EQ(sink.executions.size(), 1);
  EXPECT_EQ(sink.executions[0].price, 100);
  EXPECT_EQ(sink.executions[0].buyer, 2);
  EXPECT_EQ(sink.executions[0].seller, 1);
}

class PipelineTest : public ::testing::Test {
 protected:
  void SetUp() override {
    m_path =
        std::filesystem::path{::testing::TempDir()} /
        (std::string{
             ::testing::UnitTest::GetInstance()->current_test_info()->name()} +
         ".log");
    std::filesystem::remove(m_path);
  }

  void TearDown() override { std::filesystem::remove(m_path); }

  std::filesystem::path m_path;
};

namespace {

class TeeSink : public ob::ExecutionSink {
 public:
  TeeSink(ob::ExecutionSink* sink_a, ob::ExecutionSink* sink_b)
      : m_sink_a(sink_a), m_sink_b(sink_b) {}

  void onExecuted(const ob::ExecutedOrder& eo) override {
    m_sink_a->onExecuted(eo);
    m_sink_b->onExecuted(eo);
  }

 private:
  ob::ExecutionSink *m_sink_a, *m_sink_b;
};

}  // namespace

TEST_F(PipelineTest, LogFileMatchesExecutions) {
  /* Sink A */
  auto result = ob_logger::Logger::create(m_path);
  ASSERT_TRUE(result.has_value());
  auto logger = std::move(result.value());

  /* Sink B */
  RecordingSink recordingSink;

  /* Sinks A and B */
  TeeSink sink{logger.get(), &recordingSink};

  og::OrderGenerator orderGen{1};
  ob::OrderBook orderBook{};
  orderBook.setSink(&sink);

  for (auto i = 0U; i < 100000; ++i) {
    auto inputOrder = orderGen.generateOrder();
    orderBook.applyOrder(inputOrder);
  }

  {
    std::ifstream input{m_path};
    std::string line;
    std::size_t lineCount = 0U;
    while (std::getline(input, line)) {
      ++lineCount;
    }

    ASSERT_EQ(lineCount, recordingSink.executions.size());
  }

  {
    const std::regex re{
        R"(^\[[\d-]{10} [\d:]{8}\.\d{3} UTC\] OPERATION=(BUY|SELL), BUYER=(\d+), SELLER=(\d+), PRICE=(\d+), AMOUNT=(\d+)$)"};

    auto to_int = [](const std::ssub_match& sm) {
      int v{};
      std::from_chars(&*sm.first, &*sm.second, v);
      return v;
    };

    std::ifstream input{m_path};
    std::string line;
    std::size_t i = 0U;

    while (std::getline(input, line)) {
      std::smatch m;
      ASSERT_TRUE(std::regex_match(line, m, re));

      EXPECT_EQ(m[1].str(), ob::to_string(recordingSink.executions[i].type));
      EXPECT_EQ(to_int(m[2]), recordingSink.executions[i].buyer);
      EXPECT_EQ(to_int(m[3]), recordingSink.executions[i].seller);
      EXPECT_EQ(to_int(m[4]), recordingSink.executions[i].price);
      EXPECT_EQ(to_int(m[5]), recordingSink.executions[i].amount);

      ++i;
    }
  }
}
