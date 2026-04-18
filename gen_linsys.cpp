#include <omp.h>

#include <cmath>
#include <ctype.h>

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <fstream>

#include <time.h>

#include <vector>

#define ARG_NUMBER 2
#define MAX_NUMBER 100

using namespace std;

int main(int argc, char const *argv[])
{
    if (argc != ARG_NUMBER)
    {
        cout << "./gen_linsys.run order" << endl;
        exit(EXIT_FAILURE);
    }

    vector<float> X, B;
    const unsigned int N = atoi(argv[1]);
    float value, sum;

    srand(time(NULL));

    printf("%dx%d\n", N, N);

    for (size_t j = 0; j < N; j++)
    {
        value = ((((float)rand()) / (float)RAND_MAX) - 0.5) * MAX_NUMBER;
        X.push_back(value);
    }

    for (size_t i = 0; i < N; i++)
    {
        sum = 0;
        for (size_t j = 0; j < N; j++)
        {
            value = ((((float)rand()) / (float)RAND_MAX) - 0.5) * MAX_NUMBER;
            printf("%f; ", value);
            sum += X[j] * value;
        }
        B.push_back(sum);
    }
    printf("\n");

    printf("%dx%d\n", N, 1);

    for (size_t j = 0; j < N; j++)
    {
        printf("%f; ", B[j]);
    }
    printf("\n");
    return 0;
}