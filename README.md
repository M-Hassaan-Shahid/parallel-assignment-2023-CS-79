# Parallel Assignment: Sorted Linked List Construction

## Student Information
- **Name**: Muhammad Hassaan Shahid
- **Roll Number**: 2023-CS-79
- **Course**: Parallel and Distributed Computing
- **Instructor**: Waqas Ali

## Problem Description
A sorted linked list is built by inserting N random integers one at a time,
maintaining ascending order at each step. Originally implemented in the Data
Structures course (2nd semester), this problem has O(N²) sequential complexity
and is an ideal candidate for parallel divide-and-conquer.

## How to Run

### Sequential Version
```bash
gcc -O2 -o sequential sequential/sequential.c
./sequential -n 50000
```

### Parallel Version
```bash
gcc -O2 -pthread -o parallel parallel/parallel.c
./parallel -n 50000 -p 4
```

Or use the Makefile inside `parallel/`:
```bash
cd parallel
make          # build both
make run      # run with defaults (N=50000, P=4)
make bench    # benchmark across P=1,2,4,8
```

## Results Summary
- **Best speedup achieved**: 93.23x with P=8 threads at N=50000
- **Efficiency at P=8**: 1165% (super-linear due to O(N²) → O(N²/P²) scaling)
- **Merge overhead**: < 2% of total time across all configurations

## Performance Table (N=50000)

| Config   | Time (s) | Speedup | Efficiency |
|----------|----------|---------|------------|
| Seq      | 7.6764   | 1.00x   | 100%       |
| P=1      | 7.7872   | 0.99x   | 98.6%      |
| P=2      | 1.5001   | 5.12x   | 255.9%     |
| P=4      | 0.2770   | 27.71x  | 692.7%     |
| P=8      | 0.0823   | 93.23x  | 1165.4%    |

## Repository Structure
- `sequential/` — original sequential implementation (O(N²) baseline)
- `parallel/` — Pthreads parallel implementation with Makefile
- `results/` — timing data CSV and performance plots
- `report/` — final report PDF
