#include <cuda_runtime.h>

#include <iostream>
#include <vector>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>

#include "vector.hpp"
#include "matrix.hpp"

#define ARG_NUMBER 5

using namespace std;

float elapsed_time;
cudaEvent_t time_start;
cudaEvent_t time_end;

void init_timer();
void finish_timer();

int load_file(string filename, Matrix<float> &A, vector<float> &vec);

__global__ void CalculateXNewKernel(float *A, float *B, float *X, float *X_new, size_t x_size);
__global__ void CalculateErrorUpdateXKernel(float *X, float *X_new, float *error, size_t x_size);

int main(int argc, char const *argv[])
{
    if (argc != ARG_NUMBER)
    {
        cout << "./paralelo.run file_matrix_A file_vector_B iterations epsilon thread_number" << endl;
        exit(EXIT_FAILURE);
    }

    // TODO: Talvez fazer uma classe com MatrixDevice qual ja trabalha com cudaMalloc e cudaFree
    // para nao ter trabalhar com indice row*i + j e verificar barreiras

    Matrix<float> A;           // Host
    vector<float> B, X, X_new; // Host

    float *d_A;     // Device
    float *d_B;     // Device
    float *d_X;     // Device
    float *d_X_new; // Device

    if (load_file(argv[1], A, B))
        exit(EXIT_FAILURE);

    X.assign(B.size(), 0.0);
    X_new.assign(B.size(), 0.0);

    const unsigned int iterations = atoi(argv[2]);
    const float epsilon = atof(argv[3]);
    const unsigned int thread_number = atoi(argv[4]);

    // TODO: Decidir com sera divido os blocos
    const unsigned int block_number = (B.size() + thread_number - 1) / thread_number;

    printf("Ordem do Sistema: %d X %d\n", A.rows_number(), A.cols_number());
    printf("Maximo de Iteracoes: %d\n", iterations);
    printf("Epsilon: %f\n", epsilon);
    printf("Numero de Blocos: %d\n", block_number);
    printf("Numero de Threads: %d\n", thread_number);

    // Cuda malloc
    cudaMalloc((void **)&d_A, sizeof(float) * A.cols_number() * A.rows_number());
    cudaMalloc((void **)&d_B, sizeof(float) * B.size());
    cudaMalloc((void **)&d_X, sizeof(float) * X.size());
    cudaMalloc((void **)&d_X_new, sizeof(float) * X_new.size());

    unsigned int n = B.size();
    unsigned int iter = 0;

    float error;               // Host
    float *error_array = NULL; // Host
    float *d_error;            // Device

    error_array = (float *)calloc(sizeof(float), block_number);
    if (error_array == NULL)
        goto ERROR;

    cudaMalloc((void **)&d_error, sizeof(float) * block_number);

    // TODO: Fazer a copia de valores.

    // Todo trabalho realizado na GPU, somente copia X ao final.
    init_timer();
    do
    {
        error = 0;
        cudaMemcpy(d_error, &error, sizeof(float), cudaMemcpyHostToDevice);
        cudaDeviceSynchronize();

        // CalculateNewXKernel<<BLOCKS, THREADS>>(PARAMS);

        // cudaMemcpy(X_new.data(), d_X_new, sizeof(float) * X.size(), cudaMemcpyDeviceToHost);

        // CalculateErrorUpdateXKernel<<BLOCKS, THREADS>>(PARAMS);
        cudaMemcpy(&error, d_error, sizeof(float), cudaMemcpyDeviceToHost);
        cudaDeviceSynchronize();

        iter++;
    } while (iter < iterations && error < epsilon);
    finish_timer();

    cudaMemcpy(X.data(), d_X, sizeof(float) * X.size(), cudaMemcpyDeviceToHost);

    printf("Iteracoes: %d\n", iter);
    printf("Delta X: %f\n", error);
    printf("Tempo: %.3f\n", elapsed_time); // Nao lembro se ele da em segundos ou milisegundos

    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_X);
    cudaFree(d_X_new);

    return 0;
}

__global__ void CalculateXNewKernel(float *A, float *B, float *X, float *X_new)
{
    // Talvez fazer sum ser um variavel _shared
    // ou fazer cada kernel calcular seu proprio sum
}

__global__ void CalculateErrorUpdateXKernel(float *X, float *X_new, float *error)
{
    // Error ser shared? ou ter um vetor qual e somado ao final
    // Precisa tratar concorrencia e corrida.
}

void init_timer()
{
    cudaEventCreate(&time_start);
    cudaEventCreate(&time_end);
    cudaEventRecord(time_start, 0);
}

void finish_timer()
{
    cudaEventRecord(time_end, 0);
    cudaEventSynchronize(time_end);

    cudaEventElapsedTime(&elapsed_time, time_start, time_end);
}