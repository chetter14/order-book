# Milestones

- **M1. Correct single-threaded core.**
    - Matching engine handles buy and sell orders.
    - Add unit tests for straightforward and edge cases.
    - Add cmake build options: debug/release, gcc/clang, with sanitizers, with code coverage.
    - Write a script for getting coverage results in `.html` format. Add tests (or fix code) to get >=95% of lines coverage. 
    
- **M2. Driven end to end.**
    - Add order generator and logger. A complete pipeline (but single-threaded). 

- **M3. Measurable.**
    - Add benchmarking, throughput, etc. Add profiling.

- **M4. Concurrent.**
    - Add multithreading, SPSC queue, etc. 
    - Fix sanitizer errors.

- **M5. Optimizations. Results.**
    - Do optimizations to get better results at performance.
    - Conclude the project, complete READMEs.