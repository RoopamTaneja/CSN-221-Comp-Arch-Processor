#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <sstream>
using std::cout, std::cin, std::string;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Incorrect Format\n";
        return 0;
    }
    string traceFile = argv[1];
    string outFile = argv[2];
    std::ifstream inTrace(traceFile);
    std::ofstream outData(outFile, std::ios::trunc);
    string traceLine;
    while (getline(inTrace, traceLine))
    {
        std::stringstream ss(traceLine);
        string token;
        getline(ss, token, ' ');
        getline(ss, token, ' ');
        outData << token << " ";
        getline(ss, token, ' ');
        outData << token << " ";
        getline(ss, token, ' ');
        outData << token << "\n";
    }
    inTrace.close();
    outData.close();
    return 0;
}