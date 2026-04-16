#ifndef __MATRIX_UTILS_H
#define __MATRIX_UTILS_H

#include <iostream>
#include <string>
#include <fstream>

#include <vector>

#include <ctype.h> //Funções de caracteres

using namespace std;

template <typename K>
class Matrix
{
private:
    // string input;
    vector<K> matrix;
    unsigned int cols;
    unsigned int rows;

public:
    Matrix(string filename);
    // ~Matrix();

    void set(unsigned int row, unsigned int col, K value);
    K get(unsigned int row, unsigned int col);

    unsigned int cols_number();
    unsigned int rows_number();
};

/*
Formato de arquivo para Matrix:
NxM
item_1_1; ... ; item_N_1; . . .; item 1_M; ... ; item_N_M;

Formato de arquivo para Vetor: M = 1
*/

template <typename K>
Matrix<K>::Matrix(string filename)
{
    FILE *fp = freopen(filename.c_str(), "r", stdin);
    if (fp != nullptr)
    {
        unsigned int row_input, col_input;
        float value;

        scanf("%ux%u\n", &row_input, &col_input);
        this->rows = row_input;
        this->cols = col_input;

        while (scanf("%f;", &value) != EOF)
        {
            this->matrix.push_back(value);
        }
    }
    else
    {
        cout << "Nao foi possivel abrir o arquivo: " << filename << endl;
    }

    fclose(fp);
}

// template <typename K>
// Matrix<K>::~Matrix()
// {
//     delete[] this->matrix;
// }

template <typename K>
void Matrix<K>::set(unsigned int row, unsigned int col, K value)
{
    unsigned int pos = row * this->cols + col;
    this->matrix[pos] = value;
}

template <typename K>
K Matrix<K>::get(unsigned int row, unsigned int col)
{
    unsigned int pos = row * cols_number() + col;
    return this->matrix[pos];
}

template <typename K>
unsigned int Matrix<K>::rows_number()
{
    return this->rows;
}

template <typename K>
unsigned int Matrix<K>::cols_number()
{
    return this->cols;
}
#endif