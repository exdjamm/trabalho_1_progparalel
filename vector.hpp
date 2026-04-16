#ifndef __VECTOR_UTILS_H
#define __VECTOR_UTILS_H

#include <iostream>
#include <string>
#include <fstream>

#include <vector>

using namespace std;

template <typename K>
class Vector
{
private:
    // string input;
    vector<K> array;
    unsigned int size;

public:
    Vector(string filename);
    // ~Vector();

    void set(unsigned int pos, K value);
    K get(unsigned int pos);

    unsigned int size_number();
};

template <typename K>
Vector<K>::Vector(string filename)
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

// template <typename K>
// Vector<K>::~Vector()
// {
//     delete[] this->array;
// }

template <typename K>
unsigned int Vector<K>::size_number()
{
    return this->size;
}

template <typename K>
void Vector<K>::set(unsigned int pos, K value)
{
    this->array[pos] = value;
}

template <typename K>
K Vector<K>::get(unsigned int pos)
{
    return this->array[pos];
}

#endif