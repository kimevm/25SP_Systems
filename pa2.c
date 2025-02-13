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



double get_mean_of_uniform_random_samples()
{
    double sum = 0.0;
    for(int i = 0; i < SAMPLES; i++){
        sum += ((double)rand() / RAND_MAX) * 2.0 - 1.0; //creating random numbers from -1 --> 1
    }

    double avg = sum / (double)SAMPLES; //getting the average of al of them

    return avg;
}

double populate_values_and_get_mean(double *values)
{
    double sum = 0.0;

    for (int i = 0; i< RUNS; i++){
        values[i] = get_mean_of_uniform_random_samples(); //making an array of averages of unfiform random samples
        sum += values[i];
    }

    return sum / RUNS; //finding the average of all of the averages
}

double get_mean_squared_error(double values[], double mean)
{
    double summation = 0.0;
    for(int i = 0; i < RUNS; i++) //this is just calculating the MSE
    {
        summation+= pow(mean - values[i], 2);
    }

    return summation / RUNS;
}

void create_histogram(double values[], int *counts){
    double bin_size = HISTOGRAM_SPAN / BINS; //finding how big each bin is, to determine where each of the averages fall on the spectrum

    for (int i = 0; i < RUNS; i++)
    {
        int idx = (values[i] + (HISTOGRAM_SPAN / 2.0)) / bin_size; //a bit of translation so that it looks good, this is defined in assignment

        if (idx < 0){ //edge case
            idx = 0;
        }
        else if (idx >= BINS){ //edge case
            idx = BINS - 1;
        }
        counts[idx] += 1;
    }
}

void print_histogram(int counts[])
{
    double bin_start = -HISTOGRAM_SPAN / 2.0;
    double bin_size = HISTOGRAM_SPAN / BINS;

    for(int i = 0; i < BINS; i++){

        printf("%.4f: ", ((2*bin_start) + bin_size) / 2); //printing out the middle value of the bin, like if bin is 5 --> 10, then it shows 7.5
        for (int j = 0; j < counts[i]/SCALE; j++)
        {
            printf("X"); //printing x for the number of counts divided by the scale
        }
        printf("\n");
        bin_start += bin_size;
    }
}


int main(){

    double *values = malloc(RUNS * sizeof(double)); //allocating memory a specific number of bytes()
    int *counts = calloc(BINS, sizeof(int)); //allocating memory and initilizing values with 0's. 

    double avg = populate_values_and_get_mean(values); //avg of all the values (avgs of unif rand samples)
    double mse = get_mean_squared_error(values, avg); //mse of the array
    
    create_histogram(values, counts);
    print_histogram(counts);

    free(values); //freeing up memory
    free(counts);

    printf("Sample mean over %d means of %d samples each: %f    Sample variance: %f\n", RUNS, SAMPLES, avg, mse); //info
    return 0;
}

