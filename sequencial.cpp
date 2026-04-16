#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "vector.hpp"
#include "matrix.hpp"

#define ARG_NUMBER 5

using namespace std;

int load_vector(string filename, vector<float> vec);

int main(int argc, char const *argv[])
{
    if (argc != ARG_NUMBER)
    {
        cout << "./sequencial.run file_matrix_A file_vector_B iterations epsilon" << endl;
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

    // printf("%s, %s, %s, %s\n", argv[1], argv[2], argv[3], argv[4]);
}

int load_vector(string filename, vector<float> vec)
{
    ifstream fileInput(filename, ios::in);
    string line;

    if (fileInput.is_open())
    {
        while (getline(fileInput, line))
        {
            // scanf
        }
        fileInput.close();
    }
    else
        cout << "Nao foi possivel abrir o arquivo: " << filename << endl;
}