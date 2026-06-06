#include <mpi.h>

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "matrix.hpp"

using namespace std;

#define ARG_NUMBER 5

int load_file(string filename, Matrix<float> &A, vector<float> &B);

int main(int argc, char const *argv[])
{
    if (argc != ARG_NUMBER)
    {
        cout << "./jacobi matrix_file iterations epsilon n_process" << endl;
        return EXIT_FAILURE;
    }

    Matrix<float> A;
    vector<float> B;

    // if(myrank == 0) ???
    // Somente o processo principal le o arquivo e carrega A e B.
    if (load_file(argv[1], A, B))
    {
        return EXIT_FAILURE;
    }

    // TODO: Copiar vetor A e B para todos os processos e iniciar openmpi

    // Acho que essas operacoes nao tem problema pois o openmpi recebe argv e argc.
    const unsigned int iterations = atoi(argv[2]);
    const float epsilon = atof(argv[3]);
    const unsigned int process_number = atoi(argv[4]);

    const unsigned int n = B.size();

    const unsigned int items_per_process = (n + process_number - 1) / process_number; // ??
    /*
     Possivel Logica
     Cada processo mantem uma copia de A e B, transferido por um processo principal.
     Cada processo sera responsavel por i items do vetor X.
     Cada processo tem X_old, usado para calcular seu X_new(s).
     Ao fim de calcular X_new, manda seu valor para os outros processadores
     e espera o recebimento dos outros. (Verificar condicao de corrida e deadlocks)
     Por fim, calcula seu erro e envia para o processo principal, qual calcula
     o erro global e envia para os outros processos o valor da condicao de parada.
     Processos entram em espera ate o recebimento do valor da condicao de parada.
     Loop.
    */

    return 0;
}
