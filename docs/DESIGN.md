## Goals

1) An order book that is capable of processing buy and sell requests at a specific price.
2) Executed orders are logged into the file.
3) Generator of orders is running in one process with order book matching engine, but in a separate thread.
4) Get hands on a serious project development in C++, including: planning and design of the program, full cmake usage, various build types, sanitizers, unit tests, code coverage, benchmarks.

## Non-goals

1) No GUI.
2) No any networking.
3) No using latest C++ features (i.e., modules).
4) No databases for logging.
5) No focus on high-performance and fighting for nanoseconds.

## Target numbers

1) Generator of orders produces 100'000 orders per second.
2) Order book processes ~100'000 orders per second.
