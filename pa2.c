//TODO make the main eaccept arguments and check if we are testing. 
//     then use srand for seeded random values

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

//defined in the assignemnt
#define SAMPLES 10000
#define RUNS 50000
#define BINS 64
#define HISTOGRAM_SPAN 0.05
#define SCALE 32

double get_mean_of_uniform_random_samples(void) {

    double sum = 0.0;

    for (int i = 0; i < SAMPLES; i++) {
        /* Change: Added explicit cast to ensure floating-point division */
        double r = ((double)rand() / (double)RAND_MAX) * 2.0 - 1.0;  //creating random numbers from -1 --> 1

        sum += r;
    }

    return sum / SAMPLES; // simplified
}

double populate_values_and_get_mean(double *values) {
   
    double sum = 0.0;

    for (int i = 0; i< RUNS; i++){
        values[i] = get_mean_of_uniform_random_samples(); //making an array of averages of unfiform random samples
        sum += values[i];
    }

    return sum / RUNS; //finding the average of all of the averages
}

double get_mean_squared_error(double values[], double mean) {

    double summation = 0.0;

    for(int i = 0; i < RUNS; i++) { //this is just calculating the MSE
	double diff = mean - values[i];    
        summation += diff * diff; // pow error change
    }

    return summation / RUNS;
}

void create_histogram(double values[], int *counts) {
    double bin_size = HISTOGRAM_SPAN / BINS; //finding how big each bin is, to determine where each of the averages fall on the spectrum

    for (int i = 0; i < RUNS; i++) {
        /* Change: Compute bin index using translation to match span */
        int idx = (int)((values[i] + (HISTOGRAM_SPAN / 2.0)) / bin_size); //a bit of translation so that it looks good, this is defined in assignment

        if (idx < 0) { //edge case
            idx = 0;
        }
        else if (idx >= BINS ){ //edge case
            idx = BINS - 1;
        }

        counts[idx] += 1;
    }
}

void print_histogram(int counts[]) {

    double bin_start = -HISTOGRAM_SPAN / 2.0;
    double bin_size = HISTOGRAM_SPAN / BINS;

    for(int i = 0; i < BINS; i++){
        /* Change: Calculate center as bin_start plus half the bin size */
        double bin_center = bin_start + bin_size / 2.0;
        printf("%.4f ", bin_center);
        for (int j = 0; j < counts[i] / SCALE; j++) {
            printf("X"); //printing x for the number of counts divided by the scale
        }
        printf("\n");
        bin_start += bin_size;
    }
}


int main(int argc, char *argv[]) {

    /* Command-line argument for seeding the random generator.
       If "test" is provided as an argument, seed with a constant value for reproducibility */

    if (argc >= 2 && strcmp(argv[1], "test") == 0) {
        srand(1);  // Change: Use constant seed for testing
    } else {
        srand((unsigned)time(NULL));  // Change: Use current time for seeding
    }

    /* Allocate memory for RUNS averages */
    double *values = malloc(RUNS * sizeof(double));
    if (values == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for values array.\n");
        return 1;
    }

    int *counts = calloc(BINS, sizeof(int));  // Change: Using calloc to zero-initialize the array
    if (counts == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for counts array.\n");
        free(values);  // Change: Free allocated memory before exiting due to error
        return 1;
    }

    double avg = populate_values_and_get_mean(values); //avg of all the values (avgs of unif rand samples)
    double mse = get_mean_squared_error(values, avg); //mse of the array
    
    create_histogram(values, counts);
    print_histogram(counts);

    printf("Sample mean over %d means of %d samples each: %.6f   Sample variance: %.6f\n", RUNS, SAMPLES, avg, mse);

    free(values); //freeing up memory
    free(counts);

    return 0;
}
