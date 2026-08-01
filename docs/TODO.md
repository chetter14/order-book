# Milestones

- **M1. Correct single-threaded core.**
    - Matching engine handles buy and sell orders.
    - Add unit tests for straightforward and edge cases.
    - Add cmake build options: debug/release, gcc/clang, with sanitizers, with code coverage.
    - Write a script for getting coverage results in `.html` format. Add tests (or fix code) to get >=95% of lines coverage. 
    
- **M2. Driven end to end.**
    - Implement an order generator. Write unit-tests. Get >=95% of lines coverage.
    - Implement a logger. Write unit-tests. Get >=95% of lines coverage.
    - Connect modules with each other (to a single thread app).
    - Write integration tests (to check that the complete pipeline works as expected).

- **M3. Measurable.**
    - Add benchmarking and measurements of performance. 
    - Add a possibility of profiling the whole program.

- **M4. Concurrent.**
    - Implement multithreading. Add SPSC queue (for communication between the order generator and order book).
    - Fix TSan errors.

- **M5. Optimizations. Results.**
    - Optimize code to get at least x2 in performance. Run a profiler to find bottlenecks.
    - Conclude the project, complete READMEs.