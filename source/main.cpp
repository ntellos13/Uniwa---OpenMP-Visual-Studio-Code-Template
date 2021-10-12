#include <iostream>
#include <sstream>

using namespace std;

int main(int argc, char *argv[])
{
    #pragma omp parallel for
    for(int i = 1; i < 10; ++i)
    {
        stringstream txt;
        
        txt << i << endl;

        cout << txt.str();
    }

    return 0;
}