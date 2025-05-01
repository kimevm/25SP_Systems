/*
 * matrix_mult.c
 * Author: Lawrence Kim - kimevm@bc.edu, Nicholas Hernandez - hernantx@bc.edu 
 */

#include <errno.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

#include "matrix_mult.h"

#define USEC_IN_SEC 1000000L   
#define EPS         1e-9

static void *xmalloc(size_t nbytes)
{
    void *p = malloc(nbytes);
    if (p == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    return p;
}
void init_matrix(double *matrix, int dim)
{
    double val = 1.0;
    for (int i = 0; i < dim * dim; ++i) {
        matrix[i] = val++;
    }
}
void print_matrix(const double *matrix, int dim)
{
    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            printf("%8.1f ", matrix[i * dim + j]);
        }
        putchar('\n');
    }
}
int verify(const double *m1, const double *m2, int dim)
{
    for (int i = 0; i < dim * dim; ++i) {
        if (fabs(m1[i] - m2[i]) > EPS) {
            return FAILURE;
        }
    }
    return SUCCESS;
}
void print_verification(const double *m1,
                        const double *m2,
                        int dim,
                        const char *name)
{
    printf("Verification for %s: %s.\n",
           name,
           verify(m1, m2, dim) == SUCCESS ? "success" : "failure");
}
static struct timeval time_diff(struct timeval *start, struct timeval *end)
{
    struct timeval delta = *end;
    delta.tv_sec  -= start->tv_sec;
    delta.tv_usec -= start->tv_usec;
    if (delta.tv_usec < 0) { 
        --delta.tv_sec;
        delta.tv_usec += USEC_IN_SEC;
    }
    return delta;
}
void print_elapsed_time(struct timeval *start,
                        struct timeval *end,
                        const char *name)
{
    struct timeval d = time_diff(start, end);
    long sec = (long)d.tv_sec;
    long usec = (long)d.tv_usec;
    printf("Time elapsed for %s: %ld second%s and %ld microsecond%s.\n",
           name,
           sec,  (sec  == 1 ? "" : "s"),
           usec, (usec == 1 ? "" : "s"));
}
void multiply_chunk(const double * const a,
                    const double * const b,
                    double * const c,
                    const int dim,
                    const int row_start,
                    const int chunk)
{
    for (int i = row_start; i < row_start + chunk; ++i) {
        const int base_i = i * dim;
        for (int j = 0; j < dim; ++j) {
            double sum = 0.0;
            for (int k = 0; k < dim; ++k) {
                sum += a[base_i + k] * b[k * dim + j];
            }
            c[base_i + j] += sum;
        }
    }
}
void multiply_serial(const double * const a,
                     const double * const b,
                     double * const c,
                     const int dim,
                     const int num_workers)
{
    (void)num_workers;
    multiply_chunk(a, b, c, dim, 0, dim);
}
static pid_t fork_checked(void)
{
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    return pid;
}
static void *mmap_checked(size_t length)
{
    void *addr = mmap(NULL, length,
                      PROT_READ | PROT_WRITE,
                      MAP_ANONYMOUS | MAP_SHARED,
                      -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    return addr;
}
static void munmap_checked(void *addr, size_t length)
{
    if (munmap(addr, length) < 0) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }
}
void multiply_parallel_processes(const double * const a,
                                 const double * const b,
                                 double * const c,
                                 const int dim,
                                 const int num_workers)
{
    const size_t bytes  = (size_t)dim * dim * sizeof(double);
    double *shared_prod = (double *)mmap_checked(bytes);
    memset(shared_prod, 0, bytes);
    const int chunk = dim / num_workers;
    int row_start   = 0;
    for (int w = 0; w < num_workers - 1; ++w) {
        if (fork_checked() == 0) {
            multiply_chunk(a, b, shared_prod, dim, row_start, chunk);
            _Exit(0);
        }
        row_start += chunk;
    }
    multiply_chunk(a, b, shared_prod, dim, row_start, dim - row_start);
    while (wait(NULL) > 0) { }
    memcpy(c, shared_prod, bytes);
    munmap_checked(shared_prod, bytes);
}
static void *task(void *arg)
{
    Args *p = (Args *)arg;
    multiply_chunk(p->a, p->b, p->c, p->dim, p->row_start, p->chunk);
    return NULL;
}
void multiply_parallel_threads(const double * const a,
                               const double * const b,
                               double * const c,
                               const int dim,
                               const int num_workers)
{
    pthread_t tids[NUM_WORKERS];
    Args *arg_set = (Args *)xmalloc(sizeof(Args) * num_workers);
    const int chunk = dim / num_workers;
    int row   = 0;
    Args template = { .a = a, .b = b, .c = c, .dim = dim,
                      .row_start = 0, .chunk = chunk };
    for (int i = 0; i < num_workers; ++i) {
        arg_set[i]          = template;
        arg_set[i].row_start = row;
        arg_set[i].chunk = (i == num_workers - 1)
                           ? (dim - row) : chunk;
        row += chunk;
    }
    const int num_threads = num_workers - 1;
    for (int id = 0; id < num_threads; ++id) {
        if (pthread_create(&tids[id], NULL, task, &arg_set[id]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }
    task(&arg_set[num_workers - 1]);
    for (int id = 0; id < num_threads; ++id) {
        if (pthread_join(tids[id], NULL) != 0) {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
    }
    free(arg_set);
}
void run_and_time(multiply_function    multiply_fn,
                  const double * const a,
                  const double * const b,
                  double * const c,
                  const double * const gold,
                  const int dim,
                  const char * const name,
                  const int num_workers,
                  const bool do_verify)
{
    struct timeval start, end;
    printf("Algorithm: %s with %d worker%s.\n",
           name, num_workers, (num_workers == 1 ? "" : "s"));
    gettimeofday(&start, NULL);
    multiply_fn(a, b, c, dim, num_workers);
    gettimeofday(&end, NULL);
    print_elapsed_time(&start, &end, name);
    if (do_verify) {
        print_verification(c, gold, dim, name);
    }
}

