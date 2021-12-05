#include <iostream>

class FileParser 
{
    private:
        std::string filePath;
    public:
        FileParser(std::string filePath);

        bool TryParse(int**& out, int& length);
};