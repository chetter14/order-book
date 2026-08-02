#ifndef ORDER_BOOK_EXECUTION_SINK_H
#define ORDER_BOOK_EXECUTION_SINK_H

#include "custom_types.h"

namespace ob {
class ExecutionSink {
 public:
  virtual ~ExecutionSink() = default;
  virtual void onExecuted(const ExecutedOrder&) = 0;
};
}  // namespace ob

#endif  // ORDER_BOOK_EXECUTION_SINK_H