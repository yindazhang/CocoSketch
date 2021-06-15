CPU Code
============

Repository structure
--------------------
*  `Common/`: the hash and mmap functions
*  `Struct/`: the data structures, such as heap and hash table
*  `Single/`: the single-key sketching algorithms
*  `Multiple/`: the hardware and software versions of our CocoSketch and USS
*  `Benchmark.h`: the benchmarks about ARE, recall rate, and precision rate

Requirements
-------
- cmake
- g++

How to run
-------
```bash
$ cmake .
$ make
$ ./CPU your-dataset
```