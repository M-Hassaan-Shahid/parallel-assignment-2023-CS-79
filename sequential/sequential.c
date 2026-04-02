/*
 * sequential.c
 * Sorted Linked List Construction — Sequential Version
 *
 * Builds a sorted linked list by inserting N random integers one by one.
 * This is the baseline we will later parallelise.
 *
 * Compile:  gcc -O2 -o sequential sequential.c
 * Run:      ./sequential -n 100000
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ─── Node definition ─────────────────────────────────────── */
typedef struct Node {
    int value;
    struct Node *next;
} Node;

/* ─── List definition ─────────────────────────────────────── */
typedef struct {
    Node *head;
    int   size;
} SortedList;

/* ─── Helpers ─────────────────────────────────────────────── */

/* Allocate and return a new node */
Node *make_node(int value) {
    Node *n = (Node *)malloc(sizeof(Node));
    if (!n) { fprintf(stderr, "malloc failed\n"); exit(1); }
    n->value = value;
    n->next  = NULL;
    return n;
}

/* Insert value into list, maintaining ascending sorted order */
/* AI-Assisted: Developed with Claude (Anthropic). Logic reviewed and fully understood
 * by Muhammad Hassaan Shahid (2023-CS-79). */
void insert_sorted(SortedList *list, int value) {
    Node *new_node = make_node(value);

    /* Insert at head if list is empty or value is smallest */
    if (list->head == NULL || value <= list->head->value) {
        new_node->next = list->head;
        list->head     = new_node;
        list->size++;
        return;
    }

    /* Find the insertion point */
    Node *curr = list->head;
    while (curr->next != NULL && curr->next->value < value)
        curr = curr->next;

    new_node->next = curr->next;
    curr->next     = new_node;
    list->size++;
}

/* Merge two sorted linked lists into one sorted list.
 * Does NOT allocate new nodes — rewires existing next pointers. */
/* AI-Assisted: Developed with Claude (Anthropic). Logic reviewed and fully understood
 * by Muhammad Hassaan Shahid (2023-CS-79). */
Node *merge_sorted(Node *a, Node *b) {
    Node dummy;           /* Temporary head to simplify edge cases */
    Node *tail = &dummy;
    dummy.next = NULL;

    while (a != NULL && b != NULL) {
        if (a->value <= b->value) { tail->next = a; a = a->next; }
        else                      { tail->next = b; b = b->next; }
        tail = tail->next;
    }
    tail->next = (a != NULL) ? a : b;
    return dummy.next;
}

/* Free all nodes in a list */
void free_list(Node *head) {
    while (head) {
        Node *tmp = head;
        head = head->next;
        free(tmp);
    }
}

/* Verify list is actually sorted — returns 1 if OK, 0 if broken */
/* AI-Assisted: Developed with Claude (Anthropic). Logic reviewed and fully understood
 * by Muhammad Hassaan Shahid (2023-CS-79). */
int verify_sorted(Node *head) {
    if (!head) return 1;
    Node *curr = head;
    while (curr->next) {
        if (curr->value > curr->next->value) return 0;
        curr = curr->next;
    }
    return 1;
}

/* Print first `count` values of the list */
void print_list(Node *head, int count) {
    int i = 0;
    while (head && i < count) {
        printf("%d", head->value);
        if (head->next && i < count - 1) printf(" -> ");
        head = head->next;
        i++;
    }
    if (head) printf(" -> ...");
    printf("\n");
}

/* Wall-clock time in seconds */
double now_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

/* ─── Main ────────────────────────────────────────────────── */
int main(int argc, char **argv) {

    /* Default size; override with -n <number> */
    int N = 100000;
    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "-n") == 0)
            N = atoi(argv[i + 1]);
    }

    printf("=== Sequential Sorted Linked List ===\n");
    printf("N = %d elements\n\n", N);

    /* Generate random data */
    srand(42);   /* Fixed seed so results are reproducible */
    int *data = (int *)malloc(N * sizeof(int));
    if (!data) { fprintf(stderr, "malloc failed for data\n"); return 1; }
    for (int i = 0; i < N; i++)
        data[i] = rand() % (N * 10);

    /* ── Build sorted list by inserting one by one ── */
    double t_start = now_sec();

    SortedList list = { .head = NULL, .size = 0 };
    for (int i = 0; i < N; i++)
        insert_sorted(&list, data[i]);

    double t_build = now_sec() - t_start;

    /* ── Verify and print ─────────────────────────── */
    int ok = verify_sorted(list.head);

    printf("Build time   : %.4f seconds\n", t_build);
    printf("List size    : %d nodes\n",     list.size);
    printf("Sorted check : %s\n",           ok ? "PASS" : "FAIL");
    printf("First 10     : ");
    print_list(list.head, 10);

    /* Save timing so parallel version can read and compare */
    FILE *f = fopen("seq_time.txt", "w");
    if (f) { fprintf(f, "%.6f\n", t_build); fclose(f); }

    free_list(list.head);
    free(data);
    return 0;
}
