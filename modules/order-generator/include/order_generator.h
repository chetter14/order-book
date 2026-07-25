#ifndef ORDER_GENERATOR_H
#define ORDER_GENERATOR_H

#include "order_book.h"

namespace order_generator {

void initGenerator(unsigned int seed);
ob::InputOrder generateOrder();

}  // namespace order_generator

#endif