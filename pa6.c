/*
 * Cache Locality
 * This code will show the benefits of using a cache-friendly algorithim. 
 * Specifically it will show how transposed matrix mulitplicaiotn will be quicker
 * than a standard matrix multiplicaiton due to the characteristics of the cache.
 * Author: Lawrence Kim - kimevm@bc.edu, Nicholas Hernandez - hernantx@bc.edu
 */
#include <stdio.h>
#include <stdlib.h>

#define DIM 4
#define MAX_VALUE 20
#define TRUE  1
#define FALSE 0
#define MAX_VALUE 20

void init(const int dim, int * const m) {
    for (int i = 0; i < DIM * DIM; i++) {
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
        if (c1[i] != c2[i]) {
            return FALSE;
        }
    }
    return TRUE;
}

void transpose_and_multiply(const int dim, const int * const a, int * const b, int * const c) {
    transpose(dim, b);
    multiply_transpose(dim, a, b, c);
}

void run_test(const int dim) {
    int *matrix1 = calloc(dim * dim, sizeof(int));
    int *matrix2 = calloc(dim * dim, sizeof(int));
    init(dim, matrix1);
    init(dim, matrix2);
    int *c1 = calloc(dim * dim, sizeof(int));
    int *c2 = calloc(dim * dim, sizeof(int));

    print(dim, matrix1);
    printf("\n");
    print(dim, matrix2);
    printf("\n");


    multiply(dim, matrix1, matrix2, c1);
    transpose_and_multiply(dim, matrix1, matrix2, c2);

    print(dim, c1);
    printf("\n");
    print(dim, c2);

    if (verify(dim, c1, c2)) {
        printf("Matrixes Match!");
    } else {
        printf("No Match!");
    }

    free(matrix1);
    free(matrix2);
    free(c1);
    free(c2);
    
}

struct timeval run_and_time(
    void (* mult_func)(const int, const int * const, int * const, int * const),
    const int dim,
    const int * const a,
    int * const b,
    int * const c
);

double get_speedup(struct timeval * result1, struct timeval * result2);

int main() {
    run_test(DIM);
    return EXIT_SUCCESS;
}