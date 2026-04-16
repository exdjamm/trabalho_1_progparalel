#include <omp.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "vector.hpp"
#include "matrix.hpp"

#define ARG_NUMBER 6

using namespace std;

int load_vector(string filename, vector<float> &vec);

int main(int argc, char const *argv[])
{
    if (argc != ARG_NUMBER)
    {
        cout << "./sequencial.run file_matrix_A file_vector_B iterations epsilon thread_number" << endl;
        exit(EXIT_FAILURE);
    }
    // Matrix use classe pois precisa armazenar coluna e linha para acesso.
    Matrix<float> A(argv[1]);
    vector<float> B, X;

    if (load_vector(argv[2], B))
        exit(EXIT_FAILURE);

    X.assign(B.size(), 0.0);

    const unsigned int iterations = atoi(argv[3]);
    const float epsilon = atof(argv[4]);
    const unsigned int thread_number = atof(argv[5]);

    printf("Ordem do Sistema: %d X %d\n", A.rows_number(), A.cols_number());
    printf("Maximo de Iteracoes: %d\n", iterations);
    printf("Epsilon: %f\n", epsilon);
    printf("Numero de Threads: %d\n", thread_number);
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