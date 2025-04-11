/*
 * Cache Locality
 * This code will show the benefits of using a cache-friendly algorithim. 
 * Specifically it will show how transposed matrix mulitplicaiotn will be quicker
 * than a standard matrix multiplicaiton due to the characteristics of the cache.
 * Author: Lawrence Kim - kimevm@bc.edu, Nicholas Hernandez - hernantx@bc.edu
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define MAX_VALUE 20
#define TRUE  1
#define FALSE 0
#define MICROSECOND_PER_SECOND 1000000
#define MIN_DIM_POWER 3
#define MAX_DIM_POWER 10


void init(const int dim, int * const m) {
    for (int i = 0; i < dim * dim; i++) {
        m[i] = rand() % MAX_VALUE;
    }
}

void multiply(const int dim, const int * const a, const int * const b, int * const c) {
    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            int product_summation = 0;
            for (int k = 0; k < dim; ++k) {
                product_summation += a[i * dim + k] * b[k * dim + j];
            }
            c[i * dim + j] = product_summation;
        }
    }
}

void print(const int dim, const int * const m) {
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            printf("%4d ", m[i * dim + j]);
        }
        printf("\n");
    }
}

void transpose(const int dim, int * const m) {
    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < i; ++j) {
            int tmp = m[i * dim + j];
            m[i * dim + j] = m[j * dim + i];
            m[j * dim + i] = tmp;
        }
    }
}

void multiply_transpose(const int dim, const int * const a, const int * const b_t, int * const c) {
    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            int product_summation = 0;
            for (int k = 0; k < dim; ++k) {
                product_summation += a[i * dim + k] * b_t[j * dim + k];
            }
            c[i * dim + j] = product_summation;
        }
    }
}

int verify(const int dim, const int * const c1, const int * const c2) {
    for (int i = 0; i < dim * dim; i++) {
        if (c1[i] != c2[i])
            return FALSE;
    }
    return TRUE;
}

void transpose_and_multiply(const int dim, const int * const a, int * const b, int * const c) {
    transpose(dim, b);
    multiply_transpose(dim, a, b, c);
}

void run_test(const int dim) {
    int *a  = (int *) calloc(dim * dim, sizeof(int));
    int *b  = (int *) calloc(dim * dim, sizeof(int));
    int *c1 = (int *) calloc(dim * dim, sizeof(int));
    int *c2 = (int *) calloc(dim * dim, sizeof(int));

    if (!a || !b || !c1 || !c2) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }

    init(dim, a);
    init(dim, b);

    struct timeval tv1 = run_and_time(multiply, dim, a, b, c1);
    struct timeval tv2 = run_and_time(transpose_and_multiply, dim, a, b, c2);

    int ok = verify(dim, c1, c2);
    printf("Testing on %d-by-%d square matrices.\n", dim, dim);
    if (ok == TRUE) {
        printf("Results agree.\n");
    } else {
        printf("Results do not agree.\n");
    }
    printf("Standard multiplication: %ld seconds, %d microseconds\n",
           (long)tv1.tv_sec, (int)tv1.tv_usec);
    printf("Multiplication with transpose: %ld seconds, %d microseconds\n",
           (long)tv2.tv_sec, (int)tv2.tv_usec);

    double speedup = get_speedup(&tv1, &tv2);
    printf("Speedup: %f\n\n", speedup);

    free(a);
    free(b);
    free(c1);
    free(c2);
}

struct timeval run_and_time(
    void (* mult_func)(const int, const int * const, int * const, int * const),
    const int dim,
    const int * const a,
    int * const b,
    int * const c
) {
    struct timeval start, end, elapsed;
    gettimeofday(&start, NULL);
    mult_func(dim, a, b, c);
    gettimeofday(&end, NULL);

    elapsed.tv_sec = end.tv_sec - start.tv_sec;
    elapsed.tv_usec = end.tv_usec - start.tv_usec;
    if (elapsed.tv_usec < 0) {
        elapsed.tv_sec -= 1;
        elapsed.tv_usec += MICROSECONDS_PER_SECOND;
    }

    return elapsed;
}

double get_speedup(struct timeval * result1, struct timeval * result2) {
    double t1 = result1->tv_sec + result1->tv_usec / 1000000.0;
    double t2 = result2->tv_sec + result2->tv_usec / 1000000.0;
    return t1 / t2;
}

int main() {
    for (int power = MIN_DIM_POWER; power <= MAX_DIM_POWER; power++) {
        int dim = 1 << power;
        run_test(dim);
    }
    return EXIT_SUCCESS;
}
