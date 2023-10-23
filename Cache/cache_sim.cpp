#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <bitset>
using std::cout, std::cin, std::string, std::vector;

struct trace_data
{
    bool rw;
    long long address;
    int data;
};
class CPUreq
{
public:
    bool read_write; // 0 for read ; 1 for write
    long long tag;
    long long index;
    long long offset;
    int data;
    CPUreq(trace_data trace, int offset_size, long long offset_mask, int index_size, long long index_mask)
    {
        read_write = trace.rw;
        offset = trace.address & offset_mask;
        index = (trace.address >> offset_size) & index_mask;
        tag = trace.address >> (index_size + offset_size);
    }
};

class cache_block
{
public:
    bool valid_bit, dirty_bit;
    long long tag;
    vector<int> data_block;
    cache_block(int block_size)
    {
        valid_bit = 0;
        dirty_bit = 0;
        tag = 0;
        data_block.assign(block_size, 0);
    }
};

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        cout << "Incorrect Format\n";
        return 0;
    }
    string traceFile = argv[1];
    string paramsFile = argv[2];
    string statsFile = argv[3];

    // Main Memory Initialization
    int main_memory[1000000];
    for (int i = 0; i < 1000000; i++)
        main_memory[i] = 4 * i;

    // Cache initialization
    string paramLine;
    std::ifstream inParams(paramsFile);
    vector<int> params;
    while (getline(inParams, paramLine))
    {
        vector<string> tokens;
        std::stringstream ss(paramLine);
        string token;
        while (getline(ss, token, ':'))
            tokens.push_back(token);

        params.push_back(stoi(tokens[1]));
    }
    inParams.close();
    int cache_size = params[0];
    int ways = params[1];
    int block_size = params[2] / 4;
    string rep_policy = "LRU (Least Recently Used)";
    string write_policy = "Write Back and Write Allocate";

    ways = 1; // IMPORTANT
    int offset_size = std::__lg(block_size);
    long long offset_mask = (1 << offset_size) - 1;

    int block_num = cache_size / block_size;
    int index_size = std::__lg(block_num / ways);
    long long index_mask = (1 << index_size) - 1;
    vector<cache_block> cache(block_num, cache_block(block_size));
    int hits = 0, misses = 0, reads = 0, writes = 0, cycles = 0, instr_count = 0;

    // Reading Traces
    std::ifstream inTrace(traceFile);
    string traceLine;
    vector<trace_data> traces;
    while (getline(inTrace, traceLine))
    {
        trace_data trace;
        std::stringstream ss(paramLine);
        string token;
        getline(ss, token, ' ');
        trace.rw = (bool)(stoi(token));
        getline(ss, token, ' ');
        trace.address = stoll(token, 0, 16);
        if (trace.rw)
        {
            getline(ss, token, ' ');
            trace.data = stoll(token, 0, 16);
        }
        traces.push_back(trace);
    }
    inTrace.close();

    // Simulation
    for (auto &trace : traces)
    {
        CPUreq newRequest(trace, offset_size, offset_mask, index_size, index_mask);
        int CPUresp;
        cache_block block = cache[newRequest.index];
        if (!block.valid_bit)
        {
            // miss
            for (int i = 0; i < block_size; i++)
                block.data_block[i] = main_memory[trace.address / 4 + i];
            block.valid_bit = 1;
            block.tag = newRequest.tag;
        }
        else
        {
            if (block.tag == newRequest.tag)
            { // hit
                CPUresp = block.data_block[newRequest.offset];
            }
            else
            {
                for (int i = 0; i < block_size; i++)
                    block.data_block[i] = main_memory[trace.address / 4 + i];
                block.tag = newRequest.tag;
            }
        }
    }

    // Printing results
    cout << "Cache Size : " << cache_size << " KB\n";
    cout << "Associativity : " << ways << "\n";
    cout << "Block Size : " << block_size << " bytes\n";
    cout << "Replacement policy : " << rep_policy << "\n";
    cout << "Write policy : " << write_policy << "\n";
    cout << "Trace Name : " << traceFile << "\n";
    return 0;
}