#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

#define MATRIX_SIZE 1000 
#define NUM_RUNS 6       
#define TOLERANCE 1e-6 

typedef struct {
    const double *matrix_a;
    const double *matrix_b;
    double *matrix_c;
    int n;
    int start_row;
    int end_row;
} thread_arg_t;

double get_time() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

// Implementação Sequencial
void multiply_seq(const double *a, const double *b, double *c, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) {
                sum += a[i * n + k] * b[k * n + j];
            }
            c[i * n + j] = sum;
        }
    }
}

// Função executada por cada thread
void *worker(void *arg) {
    thread_arg_t *args = (thread_arg_t *)arg;
    int n = args->n;
    
    // Cada thread calcula apenas as suas linhas designadas
    for (int i = args->start_row; i < args->end_row; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) {
                sum += args->matrix_a[i * n + k] * args->matrix_b[k * n + j];
            }
            args->matrix_c[i * n + j] = sum;
        }
    }
    return NULL;
}

// Implementação Paralela
void multiply_par(const double *a, const double *b, double *c, int n, int thread_count) {
    pthread_t threads[thread_count];
    thread_arg_t args[thread_count];
    
    int rows_per_thread = n / thread_count;
    int extra_rows = n % thread_count;
    int current_row = 0;

    // Cria as threads e divide as linhas da matriz entre elas
    for (int i = 0; i < thread_count; i++) {
        args[i].matrix_a = a;
        args[i].matrix_b = b;
        args[i].matrix_c = c;
        args[i].n = n;
        args[i].start_row = current_row;
        
        // Distribui o resto das linhas para as primeiras threads
        int rows = rows_per_thread + (i < extra_rows ? 1 : 0);
        args[i].end_row = current_row + rows;
        current_row += rows;

        pthread_create(&threads[i], NULL, worker, &args[i]);
    }


    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }
}

// Função para comparar as matrizes e verificar
int verify_results(const double *c_seq, const double *c_par, int n) {
    for (int i = 0; i < n * n; i++) {
        if (fabs(c_seq[i] - c_par[i]) > TOLERANCE) {
            return 0; // FALHA
        }
    }
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <numero_de_threads>\n", argv[0]);
        return 1;
    }

    int thread_count = atoi(argv[1]);
    int n = MATRIX_SIZE;

    // Alocação das matrizes (Arrays 1D simulando 2D)
    double *matrix_a = malloc(n * n * sizeof(double));
    double *matrix_b = malloc(n * n * sizeof(double));
    double *matrix_c_seq = malloc(n * n * sizeof(double));
    double *matrix_c_par = malloc(n * n * sizeof(double));

    // Preenchimento das matrizes com valores aleatórios (fora do cronômetro)
    srand(42);
    for (int i = 0; i < n * n; i++) {
        matrix_a[i] = (double)rand() / RAND_MAX;
        matrix_b[i] = (double)rand() / RAND_MAX;
    }

    printf("Iniciando medições para N = %d, Threads = %d\n", n, thread_count);

    // MEDIÇÃO SEQUENCIAL (Baseline)
    double total_seq_time = 0.0;
    for (int r = 0; r < NUM_RUNS; r++) {
        double start = get_time();
        multiply_seq(matrix_a, matrix_b, matrix_c_seq, n);
        double end = get_time();
        
        if (r > 0) { // Descarta o aquecimento (r=0)
            total_seq_time += (end - start);
        }
    }
    double avg_seq_time = total_seq_time / (NUM_RUNS - 1);
    printf("Tempo Sequencial (Media): %.4f s\n", avg_seq_time);

    // MEDIÇÃO PARALELA (Pthreads)
    double total_par_time = 0.0;
    for (int r = 0; r < NUM_RUNS; r++) {
        double start = get_time();
        multiply_par(matrix_a, matrix_b, matrix_c_par, n, thread_count);
        double end = get_time();
        
        if (r > 0) { // Descarta o aquecimento
            total_par_time += (end - start);
        }
    }
    double avg_par_time = total_par_time / (NUM_RUNS - 1);
    printf("Tempo Paralelo (Media):   %.4f s\n", avg_par_time);

    // VERIFICAÇÃO DE CORRETUDE E SPEEDUP
    if (verify_results(matrix_c_seq, matrix_c_par, n)) {
        printf("Verificacao:              OK\n");
    } else {
        printf("Verificacao:              FALHA\n");
    }

    double speedup = avg_seq_time / avg_par_time;
    printf("Speedup obtido:           %.2fx\n", speedup);

    // Liberação de memória
    free(matrix_a);
    free(matrix_b);
    free(matrix_c_seq);
    free(matrix_c_par);

    return 0;
}