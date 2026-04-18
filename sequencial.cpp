#include <omp.h>

#include <cmath>
#include <ctype.h>

#include <stdio.h>
#include <stdlib.h>

#include "vector.hpp"
#include "matrix.hpp"

#define ARG_NUMBER 5

using namespace std;

int load_vector(string filename, vector<float> &vec);

int main(int argc, char const *argv[])
{
    if (argc != ARG_NUMBER)
    {
        cout << "./sequencial.run file_matrix_A file_vector_B iterations epsilon" << endl;
        exit(EXIT_FAILURE);
    }
    // Matrix use classe pois precisa armazenar coluna e linha para acesso.
    Matrix<float> A(argv[1]);
    vector<float> B, X, X_k;

    if (A.fail_load)
        exit(EXIT_FAILURE);

    if (load_vector(argv[2], B))
        exit(EXIT_FAILURE);

    X.assign(B.size(), 0.0f);
    X_k.assign(B.size(), 0.0f);

    const unsigned int iterations = atoi(argv[3]);
    const float epsilon = atof(argv[4]);

    printf("Ordem do Sistema: %d X %d\n", A.rows_number(), A.cols_number());
    printf("Maximo de Iteracoes: %d\n", iterations);
    printf("Epsilon: %f\n", epsilon);

    unsigned int iter = 0;
    float error, sum;

    double itime, ftime, exec_time;
    itime = omp_get_wtime();
    do
    {
        // Somatorio

        for (size_t i = 0; i < A.rows_number(); i++)
        {
            sum = 0;
            for (size_t j = 0; j < A.cols_number(); j++)
            {
                if (j != i)
                {
                    sum += A.get(i, j) * X[j];
                }
            }
            X_k[i] = (B[i] - sum) / A.get(i, i);
        }

        error = 0;
        for (size_t i = 0; i < X.size(); i++)
        {
            error += powf(X_k[i] - X[i], 2);
            X[i] = X_k[i];
        }

        error = sqrtf(error);
        iter++;
    } while (iter < iterations && epsilon < error);

    ftime = omp_get_wtime();
    exec_time = (ftime - itime) * 1000 * 1000;

    cout << "X = ";
    for (size_t i = 0; i < X.size(); i++)
    {
        cout << X[i] << ", ";
    }
    cout << endl;

    printf("Iteracoes: %d\n", iter);
    printf("Delta X: %f\n", error);
    printf("Tempo: %.3fus\n", exec_time);
}

int load_vector(string filename, vector<float> &vec)
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

    while (scanf("%f;", &value) != EOF)
    {
        // cout << value << endl;
        vec.push_back(value);
    }

    fclose(fp);
    return 0;
}