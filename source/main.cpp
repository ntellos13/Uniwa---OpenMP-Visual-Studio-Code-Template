#include <iostream>
#include <omp.h>

#include "FileParser.hpp"
#include "SquareMatrixConsoleLogger.hpp"
#include "Timer.hpp"

using namespace std;

bool CheckDiagonallyDominantOMPParallelForWithCollapse(int**& matrix, int& length);
bool CheckDiagonallyDominantOMPParallelFor(int**& matrix, int& length);
bool CheckDiagonallyDominant(int**& matrix, int& length);

int main(int argc, char *argv[])
{
    auto parser = FileParser("../../test.txt");
    int length = 0;
    int** matrix;

    auto timer = Timer();

    if(!parser.TryParse(matrix, length))
    {
        cout << "Invalid input table." << endl;

        return -1;
    }

    cout << "Parsed matrix in " << timer.Elapsed() << " ms." << endl;

    cout << "Maximum available Threads: " << (int)omp_get_max_threads() << endl;

    SquareMatrixConsoleLogger::Log(matrix, length);

    timer = Timer();

    auto isDiagonallyDominant = CheckDiagonallyDominant(matrix, length);

    cout << "Computed matrix diagonal dominance (Sequential) in " << timer.Elapsed() << " ms." << endl;

    timer = Timer();

    isDiagonallyDominant = CheckDiagonallyDominantOMPParallelFor(matrix, length);

    cout << "Computed matrix diagonal dominance (Parallel) in " << timer.Elapsed() << " ms." << endl;

    timer = Timer();

    isDiagonallyDominant = CheckDiagonallyDominantOMPParallelForWithCollapse(matrix, length);

    cout << "Computed matrix diagonal dominance (Parallel with collapse) in " << timer.Elapsed() << " ms." << endl;

    cout << (isDiagonallyDominant ? "Input matrix is diagonally dominant" : "Input matrix is not diagonally dominant") << endl;

    return 0;
}

bool CheckDiagonallyDominantOMPParallelForWithCollapse(int**& matrix, int& length)
{
    auto flag = true;

    int diagonalValue;
    int sum;

    #pragma omp parallel for collapse(2) private(diagonalValue, sum)
    for(auto i = 0; i < length; i++)
    {
        for(auto j = 0; j < length; j++)
        {
            if(j == 0)
            {
                diagonalValue = matrix[i][i];
                sum = diagonalValue;
            }
            
            if(i == j)
            {
                continue;
            }

            sum -= matrix[i][j];
            
            if(sum <= 0)
            {
                #pragma omp critical
                flag = flag && sum > 0;
            }
        }
    }

    return flag;
}

bool CheckDiagonallyDominantOMPParallelFor(int**& matrix, int& length)
{
    auto flag = true;

    int diagonalValue;
    int sum;

    #pragma omp parallel for private(diagonalValue, sum)
    for(auto i = 0; i < length; i++)
    {
        diagonalValue = matrix[i][i];
        sum = diagonalValue;

        for(auto j = 0; j < length; j++)
        {
            if(i == j)
            {
                continue;
            }

            sum -= matrix[i][j];
        }

        if(sum <= 0)
        {
            #pragma omp critical
            flag = flag && sum > 0;
        }
    }

    return flag;
}

bool CheckDiagonallyDominant(int**& matrix, int& length)
{
    auto flag = true;

    int diagonalValue;
    int sum;

    for(auto i = 0; i < length; i++)
    {
        if(!flag)
        {
            continue;
        }

        diagonalValue = matrix[i][i];
        sum = diagonalValue;

        for(auto j = 0; j < length; j++)
        {
            if(!flag) // since we cant break, we can finish loops ultra fast using continue statement
            {
                continue;
            }

            if(i == j)
            {
                continue;
            }

            sum -= matrix[i][j];

            if(sum <= 0)
            {
                break;
            }
        }

        if(sum <= 0)
        {
            flag = flag && sum > 0;
        }
    }

    return flag;
}
