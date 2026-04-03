# Makefile — Parallel Linked List Construction
# Usage:
#   make          — build both versions
#   make run      — run sequential then parallel (N=50000, P=4)
#   make bench    — run full benchmark across 1,2,4,8 threads
#   make clean    — remove binaries

CC      = gcc
CFLAGS  = -O2 -Wall
N       = 50000
PROCS   = 4

all: sequential parallel

sequential: ../sequential/sequential.c
	$(CC) $(CFLAGS) -o sequential ../sequential/sequential.c

parallel: parallel.c
	$(CC) $(CFLAGS) -pthread -o parallel parallel.c

run: all
	@echo "\n=== Sequential ==="
	./sequential -n $(N)
	@echo "\n=== Parallel (P=$(PROCS)) ==="
	./parallel -n $(N) -p $(PROCS)

bench: all
	@echo "\n=== Benchmark: N=$(N) ==="
	./sequential -n $(N)
	@for p in 1 2 4 8; do \
		echo "\n--- Threads: $$p ---"; \
		./parallel -n $(N) -p $$p; \
	done

clean:
	rm -f sequential parallel seq_time.txt
