#include <omp.h>

#include <iostream>
#include <vector>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>

#include "vector.hpp"
#include "matrix.hpp"

#define ARG_NUMBER 5

using namespace std;

int load_file(string filename, Matrix<float> &A, vector<float> &vec);

int main(int argc, char const *argv[])
{
    if (argc != ARG_NUMBER)
    {
        cout << "./paralelo.run file_matrix_A file_vector_B iterations epsilon thread_number" << endl;
        exit(EXIT_FAILURE);
    }

    Matrix<float> A;
    vector<float> B, X, X_new;

    if (load_file(argv[1], A, B))
        exit(EXIT_FAILURE);

    X.assign(B.size(), 0.0);
    X_new.assign(B.size(), 0.0);

    const unsigned int iterations = atoi(argv[2]);
    const float epsilon = atof(argv[3]);
    const unsigned int thread_number = atoi(argv[4]);

    omp_set_num_threads(thread_number);

    printf("Ordem do Sistema: %d X %d\n", A.rows_number(), A.cols_number());
    printf("Maximo de Iteracoes: %d\n", iterations);
    printf("Epsilon: %f\n", epsilon);
    printf("Numero de Threads: %d\n", thread_number);

    unsigned int n = B.size();
    float error;
    unsigned int iter = 0;

    double itime, ftime, exec_time;
    itime = omp_get_wtime();

    do
    {
        error = 0.0;

#pragma omp parallel for
        for (int i = 0; i < n; i++)
        {
            float sum = 0.0;

            for (int j = 0; j < n; j++)
            {
                if (j != i)
                {
                    sum += A.get(i, j) * X[j];
                }
            }

            X_new[i] = (B[i] - sum) / A.get(i, i);
        }

#pragma omp parallel for reduction(+ : error)
        for (int i = 0; i < n; i++)
        {
            error += powf(X_new[i] - X[i], 2);
            X[i] = X_new[i];
        }
        error = sqrt(error);
        iter++;

    } while (error > epsilon && iter < iterations);

    ftime = omp_get_wtime();
    exec_time = (ftime - itime) * 1000 * 1000;

    printf("\nSolução encontrada:\n");
    for (int i = 0; i < n; i++)
    {
        printf("x[%d] = %f\n", i, X[i]);
    }

    printf("Iteracoes: %d\n", iter);
    printf("Delta X: %f\n", error);
    printf("Tempo: %.3fus\n", exec_time);

    return 0;
}

int load_file(string filename, Matrix<float> &A, vector<float> &vec)
{
    FILE *fp = freopen(filename.c_str(), "r", stdin);
    if (fp == nullptr)
    {
        cout << "Nao foi possivel abrir o arquivo: " << filename << endl;
        return 1;
    }

    string line;
    unsigned int row_input, col_input;
    float value;

    scanf("%ux%u\n", &row_input, &col_input);

    A.set_cols_number(col_input);
    A.set_rows_number(row_input);

    for (size_t i = 0; i < row_input * col_input; i++)
    {
        scanf("%f;", &value);
        A.push(value);
    }

    scanf("%ux%u\n", &row_input, &col_input);

    for (size_t i = 0; i < row_input * col_input; i++)
    {
        scanf("%f;", &value);
        vec.push_back(value);
    }

    fclose(fp);
    return 0;
}