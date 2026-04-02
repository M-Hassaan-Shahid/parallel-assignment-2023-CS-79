# Sequential: Sorted Linked List Construction

## Problem Description
Builds a sorted linked list by inserting N random integers one at a time,
maintaining ascending order at every step. This problem was originally
implemented in the Data Structures course (2nd semester) as an exercise
in pointer-based data structures and in-order insertion.

- **Input**: N random integers (generated with fixed seed `42` for reproducibility)
- **Output**: Sorted linked list, build time, correctness verification
- **Complexity**: O(N²) — each insertion traverses up to the full list

## Compilation
```bash
gcc -O2 -o sequential sequential.c
```

## Usage
```bash
./sequential           # default N = 100000
./sequential -n 50000  # custom size
```

## Example Output
```
=== Sequential Sorted Linked List ===
N = 20000 elements

Build time   : 0.8724 seconds
List size    : 20000 nodes
Sorted check : PASS
First 10     : 2 -> 15 -> 23 -> 26 -> 36 -> 39 -> 61 -> 77 -> 88 -> 90 -> ...
```

## Complexity Analysis
| Operation | Complexity |
|-----------|-----------|
| Single insert (worst case) | O(N) — traverse full list |
| Build N elements | O(N²) — N insertions each O(N) |
| Memory | O(N) — one node per element |

The O(N²) build time is the bottleneck that parallelisation targets.
Timing is saved to `seq_time.txt` for the parallel version to compare against.
