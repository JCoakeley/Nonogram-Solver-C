# Nonogram Solver (C Implementation)

## Overview
This project is a C-based implementation of a **Nonogram solver**, designed for performance and efficiency.  
It uses bitset-based filtering and permutation generation to solve large puzzles.

This is a continuation/port of the original Java version, with a focus on:
- Low-level memory management
- Efficient bit operations
- Profiling & performance tuning with tools like `valgrind` and `gprof`

---

## Project Structure
```
project-root/
├── src/          # Source code (.c files)
├── include/      # Header files (.h files)
├── build/        # Compiled objects and binaries
├── tests/        # Unit tests
├── Makefile      # Build system
└── README.md     # Project documentation
```

---

## Building
To compile the project, simply run:

```bash
make
```

This will build the executable(s) into the `build/` directory.

To clean build artefacts:
```bash
make clean
```

---

## Running
Once built, you can run the solver with:
```bash
./build/nonogram <input_file>
```

---

## Development Setup
### Tools
- **Git & GitHub** → version control
- **Valgrind** → memory leak detection
- **gprof**  → profiling and performance analysis
- **gcc** → compiler toolchain


---

## Future Work
- Optimized BitSet implementation
- Tree-based permutation filtering
- Automated benchmarking scripts
- Parallelism/multithreading support
