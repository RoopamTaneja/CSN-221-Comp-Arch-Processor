#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
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
        data = trace.data;
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
        tag = -1;
        data_block.assign(block_size, 0);
    }
};

cache_block replace(vector<int> memResp, CPUreq newRequest, int block_size)
{
    // direct mapped
    cache_block newBlock(block_size);
    newBlock.tag = newRequest.tag;
    newBlock.data_block = memResp;
    return newBlock;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Incorrect Format\n";
        return 0;
    }
    string traceFile = argv[1];
    string paramsFile = argv[2];

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
    int cache_size = params[0] * 1024 / 4;
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

        if (block.tag == newRequest.tag) // hit
        {
            hits++;
            if (!newRequest.read_write) // read
                CPUresp = block.data_block[newRequest.offset], reads++;
            else
            { // write
                block.data_block[newRequest.offset] = newRequest.data;
                block.valid_bit = 1;
                block.dirty_bit = 1;
                writes++;
            }
        }
        else // miss
        {
            misses++;
            // evict
            if (block.valid_bit && block.dirty_bit)
            {
                long long addr = block.tag << (index_size + offset_size);
                addr += newRequest.index << (offset_size);
                for (int i = 0; i < block_size; i++)
                    main_memory[addr + i] = block.data_block[i];
            }
            // memRead
            vector<int> memResp;
            for (int i = 0; i < block_size; i++)
                memResp.emplace_back(main_memory[trace.address / 4 + i]);
            // replace
            block = replace(memResp, newRequest, block_size);
            cache[newRequest.index] = block;
            if (!newRequest.read_write) // read
            {
                CPUresp = block.data_block[newRequest.offset];
                block.valid_bit = 1;
                block.dirty_bit = 0;
                reads++;
            }
            else // write
            {
                block.data_block[newRequest.offset] = newRequest.data;
                block.valid_bit = 1;
                block.dirty_bit = 1;
                writes++;
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
    cout << "No of hits : " << hits << "\n";
    cout << "No of misses : " << misses << "\n";
    cout << "No of reads : " << reads << "\n";
    cout << "No of writes : " << writes << "\n";
    return 0;
}