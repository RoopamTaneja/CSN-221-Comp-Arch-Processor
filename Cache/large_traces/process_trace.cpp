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
    string traceLine;
    std::vector<long long> traces;
    long long ea = 0;
    while (getline(inTrace, traceLine))
    {
        long long diff = stoll(traceLine);
        ea = ea + diff;
        traces.push_back(ea);
    }
    inTrace.close();

    std::ofstream outData(outFile, std::ios::trunc);
    for (auto &i : traces)
        outData << "0 " << std::setw(8) << std::setfill('0') << std::hex << i << "\n";
    outData.close();
    return 0;
}