/*
 * matrix_mult.h
 * Author: Lawrence Kim - kimevm@bc.edu, Nicholas Hernandez - hernantx@bc.edu
 */
#ifndef MATRIX_MULT_H
#define MATRIX_MULT_H

#include <stdbool.h>
#include <sys/time.h>

#define DIM          1024
#define NUM_WORKERS  4
#define SUCCESS      0
#define FAILURE     -1

typedef void (*multiply_function)(const double* const a,
                                  const double* const b,
                                  double* const c,
                                  const int dim,
                                  const int num_workers);
typedef struct {
    const double* a;
    const double* b;
    double* c;
    int dim;
    int row_start;
    int chunk;
} Args;
void init_matrix(double *matrix, int dim);
void multiply_chunk(const double * const a,
                    const double * const b,
                    double * const c,
                    const int dim,
                    const int row_start,
                    const int chunk);
void multiply_serial(const double * const a,
                     const double * const b,
                     double * const c,
                     const int dim,
                     const int num_workers);
void multiply_parallel_processes(const double * const a,
                                 const double * const b,
                                 double * const c,
                                 const int dim,
                                 const int num_workers);
void multiply_parallel_threads(const double * const a,
                               const double * const b,
                               double * const c,
                               const int dim,
                               const int num_workers);
void print_elapsed_time(struct timeval *start,
                        struct timeval *end,
                        const char * const name);
int  verify(const double * const m1, const double * const m2, const int dim);
void print_verification(const double * const m1,
                        const double * const m2,
                        const int dim,
                        const char * const name);
void run_and_time(multiply_function multiply_matrices,
                  const double * const a,
                  const double * const b,
                  double * const c,
                  const double * const gold,
                  const int dim,
                  const char * const name,
                  const int num_workers,
                  const bool do_verify);

#endif
