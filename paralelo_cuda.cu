#include <cuda_runtime.h>

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "matrix.hpp"

using namespace std;

#define ARG_NUMBER 5

float elapsed_time;
cudaEvent_t time_start;
cudaEvent_t time_end;

void init_timer();
void finish_timer();

int load_file(string filename, Matrix<float> &A, vector<float> &B);

__global__ void initializeVector(float *X, size_t n);

__global__ void jacobiKernel(
    float *A,
    float *B,
    float *X,
    float *X_new,
    size_t n);

__global__ void errorKernel(
    float *X,
    float *X_new,
    float *error,
    size_t n);

int main(int argc, char const *argv[])
{
    if (argc != ARG_NUMBER)
    {
        cout << "./jacobi matrix_file iterations epsilon threads" << endl;
        return EXIT_FAILURE;
    }

    Matrix<float> A;
    vector<float> B;

    if (load_file(argv[1], A, B))
    {
        return EXIT_FAILURE;
    }

    const unsigned int iterations = atoi(argv[2]);
    const float epsilon = atof(argv[3]);
    const unsigned int threads = atoi(argv[4]);

    const unsigned int n = B.size();

    const unsigned int blocks = (n + threads - 1) / threads; // garante ter blocos suficientes para processar todos os N elementos com X threads por bloco, garante ter uma thread para cada elemento também

    vector<float> X(n, 0.0f);

    float *d_A;
    float *d_B;
    float *d_X;
    float *d_X_new;
    float *d_error;

    // garantir espaços na gpu na memória compartilhada pelos blocos -- todos blocos tem acesso
    cudaMalloc((void **)&d_A, sizeof(float) * n * n);

    cudaMalloc((void **)&d_B, sizeof(float) * n);

    cudaMalloc((void **)&d_X, sizeof(float) * n);

    cudaMalloc((void **)&d_X_new, sizeof(float) * n);

    cudaMalloc((void **)&d_error, sizeof(float) * blocks); // cada bloco calcula um erro parcial, depois soma tudo no host pra calcular o erro total

    // copia valores pra gpu
    cudaMemcpy(
        d_A,
        A.data(),
        sizeof(float) * n * n,
        cudaMemcpyHostToDevice);

    cudaMemcpy(
        d_B,
        B.data(),
        sizeof(float) * n,
        cudaMemcpyHostToDevice);

    // inicializa dx e d_X_new com 0.0f
    initializeVector<<<blocks, threads>>>(d_X, n);
    initializeVector<<<blocks, threads>>>(d_X_new, n);

    // cria buffer pra receber erros parciais de cada bloco
    vector<float> error_host(blocks);

    float error = 0.0f;

    unsigned int iter = 0;

    init_timer();

    do
    {
        jacobiKernel<<<blocks, threads>>>(
            d_A,
            d_B,
            d_X,
            d_X_new,
            n);

        // esperar tudo acabar
        cudaDeviceSynchronize();

        errorKernel<<<blocks, threads>>>(
            d_X,
            d_X_new,
            d_error,
            n);

        cudaDeviceSynchronize();

        // copia erros parciais para error_host pra calcular o erro total
        cudaMemcpy(
            error_host.data(),
            d_error,
            sizeof(float) * blocks,
            cudaMemcpyDeviceToHost);

        error = 0.0f;

        for (size_t i = 0; i < blocks; i++)
        {
            error += error_host[i];
        }

        error = sqrtf(error);

        iter++;

    } while (iter < iterations && error > epsilon);

    finish_timer();

    // traz soluções para cpu
    cudaMemcpy(
        X.data(),
        d_X,
        sizeof(float) * n,
        cudaMemcpyDeviceToHost);

    printf("X:");

    for (size_t i = 0; i < n; i++)
    {
        printf(" %f,", X[i]);
    }
    printf("\n");

    printf("\nIteracoes: %u\n", iter);
    printf("Erro: %f\n", error);
    printf("Tempo: %f ms\n", elapsed_time);

    // libera memorioa alocada na gpu
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_X);
    cudaFree(d_X_new);
    cudaFree(d_error);

    return 0;
}

__global__ void initializeVector(float *X, size_t n)
{
    // builda indice global pra threads
    size_t tid = blockIdx.x * blockDim.x + threadIdx.x;

    // não inicialiar em threads que não serão usadas (caso sobrem threads)
    if (tid >= n)
        return;

    X[tid] = 0.0f;
}

__global__ void jacobiKernel(
    float *A,
    float *B,
    float *X,
    float *X_new,
    size_t n)
{
    // builda indice global pra threads
    size_t tid = blockIdx.x * blockDim.x + threadIdx.x;

    // não fazer nada em threads que não serão usadas (caso sobrem threads)
    if (tid >= n)
        return;

    float sum = 0.0f;

    for (size_t j = 0; j < n; j++)
    {
        // ignorar, pois tid = linha do sistema, não devemos usar a diagonal
        if (j != tid)
        {
            sum += A[tid * n + j] * X[j];
        }
    }

    X_new[tid] = (B[tid] - sum) / A[tid * n + tid];
}

__global__ void errorKernel(
    float *X,
    float *X_new,
    float *error,
    size_t n)
{
    size_t tid = blockIdx.x * blockDim.x + threadIdx.x;

    // variavel compartilhada por bloco, necessário para somar erro local (threads do mesmo bloco)
    __shared__ float block_error;

    // apenas a primeira thread do bloco inicializa com zero
    if (threadIdx.x == 0)
    {
        block_error = 0.0f;
    }

    __syncthreads();

    if (tid < n)
    {
        float diff = X_new[tid] - X[tid];

        // atomic necessário dado que várias threads podem tentar escrever ao mesmo tempo
        atomicAdd(&block_error, diff * diff);

        // atualiza X
        X[tid] = X_new[tid];
    }

    __syncthreads();

    // cada bloco escreve 1 valor no vetor global de erros parciais
    if (threadIdx.x == 0)
    {
        error[blockIdx.x] = block_error;
    }
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

    // garante que tudo terminou antes de calcular o tempo
    cudaEventSynchronize(time_end);

    // calcula diferença
    cudaEventElapsedTime(
        &elapsed_time,
        time_start,
        time_end);
}

int load_file(
    string filename,
    Matrix<float> &A,
    vector<float> &B)
{
    FILE *fp =
        freopen(filename.c_str(), "r", stdin);

    if (fp == nullptr)
    {
        cout << "Erro ao abrir arquivo\n";
        return 1;
    }

    unsigned int rows;
    unsigned int cols;

    float value;

    scanf("%ux%u\n", &rows, &cols);

    A.set_rows_number(rows);
    A.set_cols_number(cols);

    for (size_t i = 0; i < rows * cols; i++)
    {
        scanf("%f;", &value);
        A.push(value);
    }

    scanf("%ux%u\n", &rows, &cols);

    for (size_t i = 0; i < rows * cols; i++)
    {
        scanf("%f;", &value);
        B.push_back(value);
    }

    fclose(fp);

    return 0;
}