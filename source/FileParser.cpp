#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>

#include "FileParser.hpp"

bool tryCreateSquareTable(std::vector<std::vector<std::string>> table, int**& out, int& length);
bool validate(std::vector<std::vector<std::string>> table);
std::vector<std::string> tokenize(std::string const &str, const char delim);

FileParser::FileParser(std::string filePath)
{
    this->filePath = filePath;
}

bool FileParser::TryParse(int**& out, int& length)
{
    std::ifstream fileStream(this->filePath, std::ios::binary);

    if(!fileStream)
    {
        return false;
    }

    fileStream.seekg(0, std::ios::end);
    std::streampos _length = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);

    std::vector<char> buffer(_length);
    fileStream.read(&buffer[0], _length);
    
    std::stringstream stringStream(buffer.data());
    std::vector<std::vector<std::string>> vector;

    std::string line;
    while (std::getline(stringStream, line))
    {
        vector.push_back(tokenize(line, ' '));
    }

    if(!validate(vector))
    {
        return false;
    }

    if(!tryCreateSquareTable(vector, out, length))
    {
        return false;
    }

    return true;
}

bool tryCreateSquareTable(std::vector<std::vector<std::string>> table, int**& out, int& length)
{
    auto size = table.size();

    auto result = new int*[size];

    for(auto i = 0; i < size; i++)
    {
        result[i] = new int[size];
        for(auto j = 0; j < size; j++)
        {
            auto data = table[i][j];

            try
            {
                result[i][j] = std::stoi(data);
            }
            catch(const std::exception& e)
            {
                return false;
            }
        }
    }

    length = size;
    out = result;

    return true;
}

bool validate(std::vector<std::vector<std::string>> table)
{
    auto length = table.size();

    if(length == 0)
    {
        return false;
    }

    for(auto _line : table)
    {
        if(_line.size() != length)
        {
            return false;
        }
    }

    return true;
}

std::vector<std::string> tokenize(std::string const &str, const char delim)
{
    std::vector<std::string> out;

    size_t start;
    size_t end = 0;
 
    while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
    {
        end = str.find(delim, start);

        out.push_back(str.substr(start, end - start));
    }

    return out;
}
