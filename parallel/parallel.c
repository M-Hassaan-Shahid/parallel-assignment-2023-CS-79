/*
 * parallel.c
 * Sorted Linked List Construction — Parallel Version (Pthreads)
 *
 * Strategy:
 *   1. Divide N elements into P equal chunks (one per thread)
 *   2. Each thread builds its own local sorted list independently — no locking
 *   3. Main thread merges all P sorted lists sequentially (merge is O(N), fast)
 *
 * Compile:  gcc -O2 -pthread -o parallel parallel.c
 * Run:      ./parallel -n 100000 -p 4
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

/* ─── Node & List ─────────────────────────────────────────── */
typedef struct Node {
    int value;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    int   size;
} SortedList;

/* ─── Thread argument struct ──────────────────────────────── */
typedef struct {
    int  thread_id;
    int *data;        /* pointer to start of this thread's chunk */
    int  count;       /* number of elements in chunk             */
    SortedList result;/* thread writes its local list here       */
} ThreadArg;

/* ─── List operations (same as sequential.c) ─────────────── */

Node *make_node(int value) {
    Node *n = (Node *)malloc(sizeof(Node));
    if (!n) { fprintf(stderr, "malloc failed\n"); exit(1); }
    n->value = value;
    n->next  = NULL;
    return n;
}

/* AI-Assisted: Developed with Claude (Anthropic). Logic reviewed and fully understood
 * by Muhammad Hassaan Shahid (2023-CS-79). */
void insert_sorted(SortedList *list, int value) {
    Node *new_node = make_node(value);
    if (list->head == NULL || value <= list->head->value) {
        new_node->next = list->head;
        list->head     = new_node;
        list->size++;
        return;
    }
    Node *curr = list->head;
    while (curr->next != NULL && curr->next->value < value)
        curr = curr->next;
    new_node->next = curr->next;
    curr->next     = new_node;
    list->size++;
}

/* Merge two sorted lists — O(a_len + b_len), no new allocation */
/* AI-Assisted: Developed with Claude (Anthropic). Logic reviewed and fully understood
 * by Muhammad Hassaan Shahid (2023-CS-79). */
Node *merge_two(Node *a, Node *b) {
    Node dummy;
    Node *tail = &dummy;
    dummy.next = NULL;
    while (a && b) {
        if (a->value <= b->value) { tail->next = a; a = a->next; }
        else                      { tail->next = b; b = b->next; }
        tail = tail->next;
    }
    tail->next = a ? a : b;
    return dummy.next;
}

void free_list(Node *head) {
    while (head) { Node *t = head; head = head->next; free(t); }
}

int verify_sorted(Node *head) {
    if (!head) return 1;
    while (head->next) {
        if (head->value > head->next->value) return 0;
        head = head->next;
    }
    return 1;
}

void print_list(Node *head, int count) {
    int i = 0;
    while (head && i < count) {
        printf("%d", head->value);
        if (head->next && i < count - 1) printf(" -> ");
        head = head->next; i++;
    }
    if (head) printf(" -> ...");
    printf("\n");
}

double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

/* ─── Thread function ─────────────────────────────────────── */
/*
 * Each thread receives a chunk of the data array.
 * It builds a local sorted list by inserting each element.
 * No shared memory is touched — zero locking needed.
 */
/* AI-Assisted: Developed with Claude (Anthropic). Logic reviewed and fully understood
 * by Muhammad Hassaan Shahid (2023-CS-79). */
void *build_local_list(void *arg) {
    ThreadArg *a = (ThreadArg *)arg;
    a->result.head = NULL;
    a->result.size = 0;

    for (int i = 0; i < a->count; i++)
        insert_sorted(&a->result, a->data[i]);

    return NULL;
}

/* ─── Main ────────────────────────────────────────────────── */
int main(int argc, char **argv) {

    int N = 100000;   /* problem size      */
    int P = 4;        /* number of threads */

    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "-n") == 0) N = atoi(argv[i+1]);
        if (strcmp(argv[i], "-p") == 0) P = atoi(argv[i+1]);
    }

    printf("=== Parallel Sorted Linked List (Pthreads) ===\n");
    printf("N = %d elements,  P = %d threads\n\n", N, P);

    /* ── Generate data (same seed as sequential for fairness) ── */
    srand(42);
    int *data = (int *)malloc(N * sizeof(int));
    if (!data) { fprintf(stderr, "malloc failed\n"); return 1; }
    for (int i = 0; i < N; i++)
        data[i] = rand() % (N * 10);

    /* ── Set up thread arguments ─────────────────────────────── */
    pthread_t  *threads = (pthread_t *)malloc(P * sizeof(pthread_t));
    ThreadArg  *args    = (ThreadArg *)malloc(P * sizeof(ThreadArg));
    if (!threads || !args) { fprintf(stderr, "malloc failed\n"); return 1; }

    int chunk    = N / P;
    int leftover = N % P;

    for (int t = 0; t < P; t++) {
        args[t].thread_id = t;
        args[t].data      = data + t * chunk;
        args[t].count     = (t == P - 1) ? chunk + leftover : chunk;
    }

    /* ── Phase 1: Parallel build ─────────────────────────────── */
    double t0 = now_sec();

    for (int t = 0; t < P; t++)
        pthread_create(&threads[t], NULL, build_local_list, &args[t]);

    for (int t = 0; t < P; t++)
        pthread_join(threads[t], NULL);

    double t_build = now_sec() - t0;

    /* ── Phase 2: Sequential merge of P sorted lists ─────────── */
    double t1 = now_sec();

    Node *merged = args[0].result.head;
    for (int t = 1; t < P; t++)
        merged = merge_two(merged, args[t].result.head);

    double t_merge = now_sec() - t1;

    double t_total = t_build + t_merge;

    /* ── Read sequential baseline time if available ─────────── */
    double t_seq = -1.0;
    FILE *f = fopen("seq_time.txt", "r");
    if (f) { fscanf(f, "%lf", &t_seq); fclose(f); }

    /* ── Verify & report ──────────────────────────────────────── */
    int ok = verify_sorted(merged);

    int total_size = 0;
    for (int t = 0; t < P; t++) total_size += args[t].result.size;

    printf("--- Timing ---\n");
    printf("Build phase  : %.4f sec  (parallel, across %d threads)\n", t_build, P);
    printf("Merge phase  : %.4f sec  (sequential)\n",                   t_merge);
    printf("Total        : %.4f sec\n",                                  t_total);

    if (t_seq > 0) {
        double speedup    = t_seq / t_total;
        double efficiency = speedup / P * 100.0;
        printf("\n--- Performance vs Sequential ---\n");
        printf("Sequential   : %.4f sec\n",    t_seq);
        printf("Speedup      : %.2fx\n",        speedup);
        printf("Efficiency   : %.1f%%\n",       efficiency);
        printf("Overhead     : merge = %.1f%% of total time\n",
               t_merge / t_total * 100.0);
    }

    printf("\n--- Correctness ---\n");
    printf("Total nodes  : %d\n",   total_size);
    printf("Sorted check : %s\n",   ok ? "PASS" : "FAIL");
    printf("First 10     : ");
    print_list(merged, 10);

    /* ── Cleanup ──────────────────────────────────────────────── */
    free_list(merged);
    free(threads);
    free(args);
    free(data);
    return 0;
}
