#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* Constants */
#define SAMPLES 10000
#define RUNS 50000
#define BINS 64
#define HISTOGRAM_SPAN 0.05
#define SCALE 32

double get_mean_of_uniform_random_samples(void) {
    double sum = 0.0;

    for (int i = 0; i < SAMPLES; i++) {
        double r = (double)rand() / (double)RAND_MAX;
        r = r * 2.0 - 1.0;
        sum += r;
    }

    return sum / SAMPLES;
}
