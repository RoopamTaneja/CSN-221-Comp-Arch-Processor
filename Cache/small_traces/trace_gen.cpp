#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Incorrect Format\n";
        return 0;
    }
    std::ofstream file(argv[1]);
    for (int i = 0; i < 10000; ++i)
    {
        bool read_write = rand() % 2; // Randomly generate read (0) or write (1)
        // long long address = rand() % 262144; // Randomly generate an address
        // Sequential access
        long long address;
        if (i % 3 == 0)
            address = i;
        // Random access
        else if (i % 3 == 1)
            address = rand() % 262144;
        // Looping access
        else
            address = i % 500; // Loop over the first 500 addresses

        int data = -1;
        if (read_write)
            data = rand() % 256; // Randomly generate data for write operation
        file << read_write << ' ' << std::setw(8) << std::setfill('0') << std::hex << address;
        if (read_write)
            file << ' ' << std::setw(4) << std::setfill('0') << std::hex << data;
        file << '\n';
    }
    file.close();

    return 0;
}
