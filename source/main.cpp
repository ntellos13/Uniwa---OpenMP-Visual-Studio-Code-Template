#include <iostream>
#include <omp.h>

#include "FileParser.hpp"
#include "SquareMatrixConsoleLogger.hpp"
#include "Timer.hpp"
#include <cmath>
#include <limits>
#include <iostream>
#include <iterator>

using namespace std;

struct Lambda 
{
    // int(*generator)(int**& matrix, int i, int j)
    template<typename TReturn, typename T>
    static TReturn lambda_ptr_exec(int**& matrix, int i, int j)
    {
        return (TReturn) (*(T*)fn<T>())(matrix, i, j);
    }

    template<typename TReturn = void, typename TemplateFunctionPointer = TReturn(*)(int**& matrix, int i, int j), typename T>
    static TemplateFunctionPointer ptr(T& t) 
    {
        fn<T>(&t);
        
        return (TemplateFunctionPointer) lambda_ptr_exec<TReturn, T>;
    }

    template<typename T>
    static void* fn(void* new_fn = nullptr) 
    {
        static void* fn;

        if (new_fn != nullptr)
        {
            fn = new_fn;
        }

        return fn;
    }
};

bool CheckDiagonallyDominantOMPParallelForWithCollapse(int**& matrix, int& length);
bool CheckDiagonallyDominantOMPParallelFor(int**& matrix, int& length);
bool CheckDiagonallyDominant(int**& matrix, int& length);
int FindAbsoluteLargestDiagonalElement(int**& matrix, int& length);
bool TryGenerateNewMatrix(int**& matrix, int& length, int**& out, int(*generator)(int**& matrix, int i, int j));
int GetMinimumWithReduction(int**& matrix, int& length);
int GetMinimumWithCritical(int**& matrix, int& length);
int GetMinimumWithBinaryTree(int**& matrix, int& length);
int GetMinimumWithBinaryTreeStep1(int**& matrix, int& length, int threadNum, int threadCount);

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        cout << "Invalid process parameters" << endl;

        return -1;
    }

    string path = argv[1];

    auto parser = FileParser(path);
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

    if(isDiagonallyDominant)
    {
        timer = Timer();

        int maxDiagnoalElement = FindAbsoluteLargestDiagonalElement(matrix, length);

        cout << "Maximum diagonal Element: " << maxDiagnoalElement << ". Computed in " << timer.Elapsed() << " ms." << endl;

        auto func = [&] (int**& __matrix, int i, int j) 
        {
            return i == j
                ? maxDiagnoalElement
                : maxDiagnoalElement - std::abs(__matrix[i][j]); 
        };

        timer = Timer();

        int** newMatrix;
        if(!TryGenerateNewMatrix(matrix, length, newMatrix, Lambda::ptr<int>(func)))
        {
            cout << "Oops. Something went wrong!" << endl;
            
            return -1;
        }

        cout << "New matrix generated in " << timer.Elapsed() << " ms." << endl;

        timer = Timer();

        int min = GetMinimumWithReduction(newMatrix, length);

        cout << "Computed minimum matrix Element (Reduction): " << min << ". Computed in " << timer.Elapsed() << " ms." << endl;

        timer = Timer();

        min = GetMinimumWithCritical(newMatrix, length);

        cout << "Computed minimum matrix Element (OMP Critical): " << min << ". Computed in " << timer.Elapsed() << " ms." << endl;

        timer = Timer();

        min = GetMinimumWithBinaryTree(newMatrix, length);

        cout << "Computed minimum matrix Element (BinaryTree): " << min << ". Computed in " << timer.Elapsed() << " ms." << endl;
    }

    return 0;
}

bool CheckDiagonallyDominantOMPParallelForWithCollapse(int**& matrix, int& length)
{
    auto flag = true;

    int diagonalValue;
    int sum;

    // this is not the optimal way, because length must be an integer multiple of num_threads.
    #pragma omp parallel for collapse(2) private(diagonalValue, sum) num_threads(8)
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

int FindAbsoluteLargestDiagonalElement(int**& matrix, int& length)
{
    int result = 0;

    #pragma omp parallel for reduction(max:result)
    for(auto i = 0; i < length; i++)
    {
        result = max(result, std::abs(matrix[i][i]));
    }

    return result;
}

int GetMinimumWithReduction(int**& matrix, int& length)
{
    int result = std::numeric_limits<int>::max();

    #pragma omp parallel for collapse(2) reduction(min:result)
    for(auto i = 0; i < length; i++)
    {
        for(auto j = 0; j < length; j++)
        {
            result = min(result, matrix[i][j]);
        }
    }

    return result;
}

int GetMinimumWithCritical(int**& matrix, int& length)
{
    int result = std::numeric_limits<int>::max();

    #pragma omp parallel for collapse(2) reduction(min:result)
    for(auto i = 0; i < length; i++)
    {
        for(auto j = 0; j < length; j++)
        {
            if(matrix[i][j] < result)
            {
                #pragma omp critical
                result = matrix[i][j];
            }
        }
    }

    return result;
}

int GetMinimumWithBinaryTree(int**& matrix, int& length)
{
    int* results = nullptr;

    #pragma omp parallel shared(results)
    {
        auto threadCount = omp_get_num_threads();
        auto threadNum = omp_get_thread_num();
        
        // init results array and synchronize threads.
        if(threadNum == 0)
        {
            results = new int[threadCount];
        }

        #pragma omp barrier

        auto stepCount = (int)std::log2(threadCount);
        
        // execute different code based on binary tree level.
        // First step: Each thread takes length / threadCount numbers, counts a local min and appends to results array.
        // Other steps: Thread_X counts 
        for(auto step = 0; step < stepCount; step++)
        {
            if(step == 0)
            {
                results[threadNum] = GetMinimumWithBinaryTreeStep1(matrix, length, threadNum, threadCount);
            }
            else
            {
                auto maxThread = threadCount >> step;

                if(threadNum < maxThread)
                {
                    results[threadNum] = std::min(results[threadNum], results[maxThread + threadNum]);
                }
            }

            #pragma omp barrier
        }
    }

    return results[0];
}

int GetMinimumWithBinaryTreeStep1(int**& matrix, int& length, int threadNum, int threadCount)
{
    int result = std::numeric_limits<int>::max();
    auto threadElements = threadNum < threadCount - 1 // is not last 
        ? length / threadCount
        : (length / threadCount) + (length % threadCount);
    auto start = threadNum * threadElements;
    auto end = (threadNum * threadElements) + threadElements;

    for(auto x = start; x < end; x++)
    {
        auto i = x / length;
        auto j = x % length;

        if(matrix[i][j] < result)
        {
            result = matrix[i][j];
        }
    }

    return result;
}

bool TryGenerateNewMatrix(int**& matrix, int& length, int**& out, int(*generator)(int**& matrix, int i, int j))
{
    auto result = new int*[length];

    #pragma omp parallel for
    for(auto i = 0; i < length; i++)
    {
        result[i] = new int[length];
    }

    #pragma omp parallel for collapse(2)
    for(auto i = 0; i < length; i++)
    {
        for(auto j = 0; j < length; j++)
        {
            result[i][j] = generator(matrix, i, j);
        }
    }

    out = result;

    return true;
}