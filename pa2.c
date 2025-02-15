#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define SAMPLES 10000
#define RUNS 50000
#define BINS 64
#define HISTOGRAM_SPAN 0.05
#define SCALE 32

double get_mean_of_uniform_random_samples(void) {

    double sum = 0.0;

    for (int i = 0; i < SAMPLES; i++) {
        double r = ((double)rand() / (double)RAND_MAX) * 2.0 - 1.0;
        sum += r;
    }

    return sum / SAMPLES;
}

double populate_values_and_get_mean(double *values) {

    double sum = 0.0;

    for (int i = 0; i< RUNS; i++){
        values[i] = get_mean_of_uniform_random_samples();
        sum += values[i];
    }

    return sum / RUNS;
}

double get_mean_squared_error(double values[], double mean) {

    double summation = 0.0;

    for(int i = 0; i < RUNS; i++) {
    	double diff = mean - values[i];    
        summation += diff * diff;
    }

    return summation / RUNS;
}

void create_histogram(double values[], int *counts) {

    double bin_size = HISTOGRAM_SPAN / BINS;

    for (int i = 0; i < RUNS; i++) {
        int idx = (int)((values[i] + (HISTOGRAM_SPAN / 2.0)) / bin_size);
       
        if (idx < 0) { 
            idx = 0;
        } else if (idx >= BINS ){ 
            idx = BINS - 1;
        }

        counts[idx] += 1;
    }
}

void print_histogram(int counts[]) {

    double bin_start = -HISTOGRAM_SPAN / 2.0;
    double bin_size = HISTOGRAM_SPAN / BINS;

    for(int i = 0; i < BINS; i++){
        double bin_center = bin_start + bin_size / 2.0;
        printf("%.4f ", bin_center);

        for (int j = 0; j < counts[i] / SCALE; j++) {
            printf("X"); 
        }

        printf("\n");
        bin_start += bin_size;
    }
}


int main(int argc, char *argv[]) {

    if (argc >= 2 && strcmp(argv[1], "test") == 0) {
        srand(1);
    } else {
        srand((unsigned)time(NULL));
    }

    double *values = malloc(RUNS * sizeof(double));

    if (values == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for values array.\n");
        return 1;
    }

    int *counts = calloc(BINS, sizeof(int));

    if (counts == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for counts array.\n");
        free(values);

        return 1;
    }

    double avg = populate_values_and_get_mean(values);
    double mse = get_mean_squared_error(values, avg);
    
    create_histogram(values, counts);
    print_histogram(counts);

    printf("Sample mean over %d means of %d samples each: %.6f   Sample variance: %.6f\n", RUNS, SAMPLES, avg, mse);

    free(values);
    free(counts);

    return 0;
}
