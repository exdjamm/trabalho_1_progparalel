#include <mpi.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>

#include "matrix.hpp"

using namespace std;

#define ARG_NUMBER 4

int load_file(string filename, Matrix<float> &A, vector<float> &B);

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv); // inicializa os processos

    int rank, process_number;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);           // define id (rank) para cada processo criado
    MPI_Comm_size(MPI_COMM_WORLD, &process_number); // define o numero total de processos criados

    if (argc != ARG_NUMBER)
    {
        if (rank == 0)
            cout << "./jacobi matrix iterations epsilon\n";

        MPI_Finalize();
        return EXIT_FAILURE;
    }

    Matrix<float> A;
    vector<float> B;

    unsigned int n = 0;

    if (rank == 0)
    {
        if (load_file(argv[1], A, B))
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE); // mata todos os processos

        n = B.size();
    }

    MPI_Bcast(&n, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD); // é bloqueante e serve para que o processo de rank 0 espalhe o valor de n para todos os processos

    unsigned int iterations = atoi(argv[2]);
    float epsilon = atof(argv[3]);

    if (n % process_number != 0)
    {
        if (rank == 0)
            cout << "Numero de linhas deve ser divisivel pelo numero de processos.\n";

        MPI_Finalize(); // cada processo encerrará
        return EXIT_FAILURE;
    }

    unsigned int localRows = n / process_number;

    vector<float> localA(localRows * n); // matriz que o processo vai trabalhar
    vector<float> localB(localRows);     // partes do vetor b que o processo vai trabalhar

    MPI_Scatter(
        rank == 0 ? A.data() : nullptr,
        localRows * n,
        MPI_FLOAT,
        localA.data(),
        localRows * n,
        MPI_FLOAT,
        0,
        MPI_COMM_WORLD); // é bloqueante e serve para que o processo de rank 0 espalhe partes da matriz A para cada processo

    MPI_Scatter(
        rank == 0 ? B.data() : nullptr,
        localRows,
        MPI_FLOAT,
        localB.data(),
        localRows,
        MPI_FLOAT,
        0,
        MPI_COMM_WORLD); // é bloqueante e serve para que o processo de rank 0 espalhe partes do vetor B para cada processo.

    vector<float> Xold(n, 0.0f);
    vector<float> Xnew(n, 0.0f);
    vector<float> localX(localRows); // cada processo vai calcular uma parte do vetor solução

    float error;
    unsigned int iter = 0;

    double start = MPI_Wtime();

    do
    {
        float localError = 0.0f;

        for (unsigned int i = 0; i < localRows; i++)
        {
            unsigned int globalRow = rank * localRows + i; // saber qual linha da matriz original o processo está trabalhando

            float sum = 0.0f;

            for (unsigned int j = 0; j < n; j++)
            {
                if (j != globalRow)
                    sum += localA[i * n + j] * Xold[j]; // i*n garante pegar elementos da linha correta, pois Local A está representado como vetor.
            }

            float diagonal = localA[i * n + globalRow];

            if (diagonal == 0.0f)
            {
                if (rank == 0)
                    cerr << "Erro: elemento diagonal nulo.\n";
                MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
            }

            localX[i] = (localB[i] - sum) / diagonal; // escreve na solução local
        }

        MPI_Allgather(
            localX.data(),
            localRows,
            MPI_FLOAT,
            Xnew.data(),
            localRows,
            MPI_FLOAT,
            MPI_COMM_WORLD); // é bloqueante e serve para que todos compartilhem suas soluções locais para formar o vetor solução completo em cada processo

        for (unsigned int i = 0; i < localRows; i++)
        {
            unsigned int globalRow = rank * localRows + i;
            float diff = Xnew[globalRow] - Xold[globalRow];
            localError += diff * diff;
        }

        MPI_Allreduce(
            &localError,
            &error,
            1,
            MPI_FLOAT,
            MPI_SUM,
            MPI_COMM_WORLD); // soma erros locais para obter o erro global

        error = sqrt(error);

        Xold.swap(Xnew);

        iter++;

    } while (iter < iterations && error > epsilon);

    double end = MPI_Wtime();

    if (rank == 0)
    {
        cout << "Iteracoes: " << iter << endl;
        cout << "Erro final: " << error << endl;
        cout << "Tempo: " << (end - start) * 1000 << " ms\n";

        cout << "\nSolucao:\n";
        for (unsigned int i = 0; i < n; i++)
            cout << "x[" << i << "] = " << Xold[i] << " ,";
        cout << endl;
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
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