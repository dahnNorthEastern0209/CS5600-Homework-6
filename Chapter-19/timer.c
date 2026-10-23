
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

int main(void) {
    struct timeval previous, current;
    long minimum_us = 1000000;
    long positive_changes = 0;

    gettimeofday(&previous, NULL);

    for (int i = 0; i < 1000000; i++) {
        gettimeofday(&current, NULL);

        long difference =
            (current.tv_sec - previous.tv_sec) * 1000000L +
            (current.tv_usec - previous.tv_usec);

        if (difference > 0) {
            if (difference < minimum_us) {
                minimum_us = difference;
            }
            positive_changes++;
        }

        previous = current;
    }

    struct timespec resolution;

    if (clock_getres(CLOCK_MONOTONIC, &resolution) == 0) {
        printf("Monotonic clock reported resolution: %ld ns\n",
               resolution.tv_sec * 1000000000L +
               resolution.tv_nsec);
    }

    printf("Smallest observed positive gettimeofday change: %ld us\n",
           minimum_us);
    printf("Positive timestamp changes: %ld\n", positive_changes);

    return 0;
}
