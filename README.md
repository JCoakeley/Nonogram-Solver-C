# Nonogram Puzzle Solver (C)

A high-performance Nonogram puzzle solver written in C. It parses standard clue input files and outputs the fully solved board using optimized bitwise solving techniques.

## Features

- **File-Based Input**: Reads board size and row/column clues from a plain text file.  
- **Efficient Permutation Generation**: Recursively builds all valid line permutations with early pruning against partial solutions.  
- **Bitwise Filtering**: Uses custom `BitSet` structures and bitmask checks to handle millions of permutations with minimal overhead.  
- **Optimized Memory Usage**: Dynamically allocates permutation arrays and reallocates to fit the exact solution space.  
- **Performance Benchmarking**: Tracks detailed timing stats (generation, filtering, solving) and supports averaged multi-run benchmarks.  
- **Scalable**: Tested on puzzles up to 50×50 with multi-million permutation counts.

## How It Works

1. Loads a text file describing the puzzle:
   ```
   <width> <height>
   <row clues...>
   <column clues...>
   ```
2. Generates permutations for each row and column using recursive generation + bit shifts.  
3. Applies early pruning, overlap logic, and filtering to iteratively solve the grid.  
4. Outputs the solved board to the console, along with performance statistics.

## Sample Input File

```
5,5
3
3
1
3
1,1,1
2,1
2
2,2
1
3
```

## Optimizations

- Bitmask-based representation of rows and columns (`uint64_t` arrays).  
- Custom C `BitSet` with tracked bounds to speed up filtering.  
- Early pruning eliminates invalid branches during permutation generation.  
- Benchmark mode runs multiple iterations for averaged profiling data.  

## Future Work

- Tree-based permutation filtering for large-scale puzzles.  
- Generation shortcuts using partial permutations + bit shifts.  
- Optional multithreading for very large puzzles.  
- Expanded puzzle input format support.  

## Example Output

```
 -------------
 | ■ ■ ■ X X |
 | ■ ■ ■ X X |
 | X X X X ■ |
 | X X ■ ■ ■ |
 | ■ X ■ X ■ |
 -------------
```
