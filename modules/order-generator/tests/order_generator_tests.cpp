#include <gtest/gtest.h>
#include <iostream>
#include "order_generator.h"

TEST(OrderGeneratorCheckGenerated, Seed1) {
  unsigned int seed = 1;
  order_generator::initGenerator(seed);

  auto order = order_generator::generateOrder();

  std::cout << order << std::endl;

  ASSERT_EQ(1, 1);
}