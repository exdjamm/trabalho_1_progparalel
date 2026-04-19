#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <ctime>

using namespace std;

#define MAX_NUMBER 5.0

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        cout << "./gen.run N output.txt\n";
        return 1;
    }

    int N = atoi(argv[1]);
    ofstream file(argv[2]);

    srand(time(NULL));

    vector<float> X(N), B(N);

    for (int i = 0; i < N; i++)
        X[i] = ((float)rand() / RAND_MAX) * 10;

    file << N << "x" << N << "\n";

    for (int i = 0; i < N; i++)
    {
        vector<float> row(N);
        float sum = 0.0;
        float row_sum = 0.0;

        for (int j = 0; j < N; j++)
        {
            if (j != i)
            {
                float val = ((float)rand() / RAND_MAX) * MAX_NUMBER;
                row[j] = val;
                row_sum += fabs(val);
                sum += val * X[j];
            }
        }

        row[i] = row_sum + 1.0;
        sum += row[i] * X[i];

        B[i] = sum;

        for (int j = 0; j < N; j++)
            file << row[j] << "; ";
        file << "\n";
    }

    file << N << "x1\n";

    for (int i = 0; i < N; i++)
        file << B[i] << "; ";

    file << "\n";

    file.close();

    cout << "Arquivo gerado com sucesso!\n";
    return 0;
}