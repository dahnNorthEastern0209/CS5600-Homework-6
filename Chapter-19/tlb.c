#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

static double elapsed_ns(struct timespec start,
                         struct timespec end) {
    return (double)(end.tv_sec - start.tv_sec) * 1000000000.0
         + (double)(end.tv_nsec - start.tv_nsec);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <pages> <trials>\n", argv[0]);
        return 1;
    }

    char *end1, *end2;

    errno = 0;
    unsigned long long pages = strtoull(argv[1], &end1, 10);
    if (errno || end1 == argv[1] || *end1 != '\0' || pages == 0) {
        fprintf(stderr, "Invalid number of pages\n");
        return 1;
    }

    errno = 0;
    unsigned long long trials = strtoull(argv[2], &end2, 10);
    if (errno || end2 == argv[2] || *end2 != '\0' || trials == 0) {
        fprintf(stderr, "Invalid number of trials\n");
        return 1;
    }

    long page_size = sysconf(_SC_PAGESIZE);

    if (page_size <= 0 ||
        (unsigned long long)page_size > SIZE_MAX ||
        pages > SIZE_MAX / (unsigned long long)page_size ||
        pages > ULLONG_MAX / trials) {
        fprintf(stderr, "Requested measurement is too large\n");
        return 1;
    }

    size_t bytes = (size_t)pages * (size_t)page_size;
    size_t jump = (size_t)page_size / sizeof(int);

    if (jump == 0) {
        fprintf(stderr, "Page size is too small\n");
        return 1;
    }

    int *memory = NULL;

    if (posix_memalign((void **)&memory,
                       (size_t)page_size, bytes) != 0) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    volatile int *a = memory;

    // Initialize each memory page before timing.
    for (size_t p = 0; p < (size_t)pages; p++) {
        a[p * jump] = 0;
    }

    // Warm-up to reduce first-access overhead.
    for (int w = 0; w < 100; w++) {
        for (size_t p = 0; p < (size_t)pages; p++) {
            a[p * jump]++;
        }
    }

    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    //For loop to  measure repeated accesses to every page.
    for (unsigned long long t = 0; t < trials; t++) {
        for (size_t p = 0; p < (size_t)pages; p++) {
            a[p * jump]++;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double total_accesses = (double)pages * (double)trials;
    double total_ns = elapsed_ns(start, end);
    double average_ns = total_ns / total_accesses;

    // CSV format: pages,trials,average nanoseconds per access
    printf("%llu,%llu,%.3f\n", pages, trials, average_ns);

    free(memory);
    return 0;
}
