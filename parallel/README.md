# Parallel: Sorted Linked List Construction (Pthreads)

## Parallelisation Approach
**Strategy: Parallel Build + Sequential Merge**

```
Data array [0 ........... N-1]
           |       |       |       |
        Thread0 Thread1 Thread2 Thread3   ← Phase 1: parallel (no locking)
           |       |       |       |
        list0   list1   list2   list3
           \       \       /       /
            \       merged        /       ← Phase 2: sequential merge O(N)
             \                   /
              final sorted list
```

Each thread builds its own local sorted list from its chunk of data
independently — no shared memory, no mutexes needed in the build phase.
The merge phase combines P sorted lists in O(N) time.

## Why Super-linear Speedup?

The sequential algorithm is O(N²). When the array is divided into P chunks
of N/P elements each, each thread performs O((N/P)²) insertions. With P
threads running in parallel, the total build time becomes O(N²/P²) — giving
a speedup proportional to P², not just P. This is a legitimate and well-known
phenomenon with quadratic algorithms and is not a measurement error.

## Compilation
```bash
gcc -O2 -pthread -o parallel parallel.c
```

## Usage
```bash
./parallel                    # default: N=100000, P=4
./parallel -n 50000 -p 2      # 50000 elements, 2 threads
./parallel -n 100000 -p 8     # 100000 elements, 8 threads
```
Run `./sequential -n <N>` first to generate `seq_time.txt` for speedup comparison.

## Example Output (N=20000, P=4)
```
=== Parallel Sorted Linked List (Pthreads) ===
N = 20000 elements,  P = 4 threads

--- Timing ---
Build phase  : 0.0352 sec  (parallel, across 4 threads)
Merge phase  : 0.0005 sec  (sequential)
Total        : 0.0357 sec

--- Performance vs Sequential ---
Sequential   : 0.8724 sec
Speedup      : 24.42x
Efficiency   : 610.4%
Overhead     : merge = 1.4% of total time

--- Correctness ---
Total nodes  : 20000
Sorted check : PASS
```

## Benchmark Results (measured on lab machine)

### N = 20000
| Config   | Time (s) | Speedup | Efficiency |
|----------|----------|---------|------------|
| Seq      | 0.8724   | 1.00x   | 100%       |
| P=1      | 0.8933   | 0.98x   | 97.7%      |
| P=2      | 0.1553   | 5.62x   | 280.8%     |
| P=4      | 0.0357   | 24.42x  | 610.4%     |
| P=8      | 0.0164   | 53.11x  | 663.8%     |

### N = 50000
| Config   | Time (s) | Speedup | Efficiency |
|----------|----------|---------|------------|
| Seq      | 7.6764   | 1.00x   | 100%       |
| P=1      | 7.7872   | 0.99x   | 98.6%      |
| P=2      | 1.5001   | 5.12x   | 255.9%     |
| P=4      | 0.2770   | 27.71x  | 692.7%     |
| P=8      | 0.0823   | 93.23x  | 1165.4%    |

## Data Distribution Strategy
- Array split into P equal chunks: `chunk = N / P`
- Last thread gets any remainder: `count = chunk + (N % P)`
- No communication between threads during build — zero synchronisation needed
- Merge is a sequential pairwise operation after all threads complete

## Synchronisation
- **Build phase**: No locking. Each thread has its own `SortedList` struct.
- **Merge phase**: Single-threaded. No race conditions possible.
- **pthread_join**: Main thread waits for all workers before merging.
