#include <gtest/gtest.h>
#include <fstream>
#include <regex>
#include <string>
#include "logger.h"

TEST(LoggerFormat, ProducesExpectedLine) {
  using namespace std::chrono;
  const sys_time<milliseconds> when{sys_days{2026y / June / 15} + hours{12} +
                                    minutes{7} + milliseconds{317}};

  ob::ExecutedOrder order{.buyer = 1,
                          .seller = 2,
                          .type = ob::OrderType::BUY,
                          .price = 33,
                          .amount = 44};

  EXPECT_EQ(ob_logger::formatExecutedOrder(order, when),
            "[2026-06-15 12:07:00.317 UTC] OPERATION=BUY, BUYER=1, SELLER=2, "
            "PRICE=33, AMOUNT=44");
}

class LoggerTest : public ::testing::Test {
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

TEST_F(LoggerTest, CorrectPath) {
  auto result = ob_logger::Logger::create(m_path);

  EXPECT_TRUE(result.has_value());
}

TEST_F(LoggerTest, WrongPath) {
  m_path = "/no_such_folder/logs";

  auto result = ob_logger::Logger::create(m_path);

  EXPECT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), ob_logger::LoggerError::FAILED_TO_OPEN_FILE);
}

TEST_F(LoggerTest, EachRecordMustMatch) {
  constexpr std::size_t RECORD_COUNT = 5U;

  {
    auto result = ob_logger::Logger::create(m_path);
    ASSERT_TRUE(result.has_value());

    auto logger = std::move(result.value());

    for (std::size_t i = 0; i < RECORD_COUNT; ++i) {
      logger->recordExecutedOrder({.buyer = 17,
                                   .seller = 31,
                                   .type = ob::OrderType::BUY,
                                   .price = 45,
                                   .amount = 98});
    }
  }

  std::ifstream file{m_path};
  ASSERT_TRUE(file.is_open());

  const std::regex re{
      R"(^\[\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d{3} UTC\] OPERATION=BUY, BUYER=17, SELLER=31, PRICE=45, AMOUNT=98$)"};

  std::string line;
  std::size_t linesCnt = 0;

  while (std::getline(file, line)) {
    EXPECT_TRUE(std::regex_match(line, re));
    ++linesCnt;
  }

  EXPECT_EQ(linesCnt, RECORD_COUNT);
}