#include <iostream>

#include "SquareMatrixConsoleLogger.hpp"

SquareMatrixConsoleLogger::SquareMatrixConsoleLogger()
{
}

void SquareMatrixConsoleLogger::Log(int**& matrix, int& length)
{
    if(length > 50)
    {
        std::cout << "Input Matrix Length: " << length << std::endl;

        return;
    }

    std::cout << "Input Matrix: " << std::endl;

    for(auto i = 0; i < length * 2 + 1; i++)
    {
        std::cout << "-";
    }

    std::cout << std::endl;

    for(auto i = 0; i < length; i++)
    {
        std::cout << "|";

        for(auto j = 0; j < length; j++)
        {
            std::cout << (j == 0 ? "" : " ") << matrix[i][j];
        }

        std::cout << "|" << std::endl;
    }

    for(auto i = 0; i < length * 2 + 1; i++)
    {
        std::cout << "-";
    }

    std::cout << std::endl;
    
    std::cout << "Length: " << length << std::endl;

}