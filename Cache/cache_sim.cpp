#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
using std::cout, std::cin, std::string, std::vector;

struct trace_data
{
    bool rw;
    long long address;
    int data = -1;
};

vector<trace_data> readTraces(string &traceFile)
{
    std::ifstream inTrace(traceFile);
    string traceLine;
    vector<trace_data> traces;
    while (getline(inTrace, traceLine))
    {
        trace_data trace;
        std::stringstream ss(traceLine);
        string token;
        getline(ss, token, ' ');
        trace.rw = (bool)(stoi(token));
        getline(ss, token, ' ');
        trace.address = stoll(token, 0, 16);
        if (trace.rw)
        {
            getline(ss, token, ' ');
            trace.data = stoi(token, 0, 16);
        }
        traces.push_back(trace);
    }
    inTrace.close();
    return traces;
}

class CPUreq
{
public:
    bool read_write;
    long long tag;
    long long index;
    long long offset;
    int data;
    CPUreq(trace_data &trace, int offset_size, long long offset_mask, int index_size, long long index_mask)
    {
        long long addr = trace.address;
        addr >>= 2; // byte address to word address
        read_write = trace.rw;
        offset = addr & offset_mask;
        index = (addr >> offset_size) & index_mask;
        tag = addr >> (index_size + offset_size);
        data = trace.data;
    }
    void getReqData()
    {
        cout << read_write << " " << tag << " " << index << " " << offset << " ";
        if (read_write)
            cout << data;
        cout << "\n";
    }
};

int main_memory[262144]; // global variable to prevent stack from filling

vector<int> memRead(trace_data &trace, int block_size, long long offset_mask)
{
    block_size /= 4; // bytes -> no of words
    long long mask = offset_mask << 2;
    mask += 3;
    mask = ~mask;
    long long addr = trace.address & mask;
    vector<int> memResp;
    for (int i = 0; i < block_size; i++)
        memResp.emplace_back(main_memory[addr / 4 + i]);
    return memResp;
}

void memWrite(long long addr, int block_size, vector<int> &data)
{
    for (int i = 0; i < block_size; i++)
        main_memory[addr / 4 + i] = data[i];
}

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

class Cache
{
public:
    vector<vector<cache_block>> cache_array;
    int cache_size;
    int block_size;
    int offset_size;
    long long offset_mask;
    int block_num;
    int set_num;
    int index_size;
    long long index_mask;

    Cache(int cache_sz, int ways, int block_sz)
    {
        cache_size = cache_sz * 256; // KB -> no of words
        block_size = block_sz / 4;   // bytes -> no of words
        offset_size = std::__lg(block_size);
        offset_mask = (1 << offset_size) - 1;
        block_num = cache_size / block_size;
        set_num = block_num / ways;
        index_size = std::__lg(set_num);
        index_mask = (1 << index_size) - 1;
        cache_array.assign(set_num, vector<cache_block>(ways, cache_block(block_size)));
    }

    void evict(cache_block &block, CPUreq &newRequest, int &dirty_evict)
    {
        if (!block.valid_bit) // do nothing for invalid block
            return;
        if (block.valid_bit && block.dirty_bit) // write back for dirty block
        {
            dirty_evict++;
            // cout << "Dirty evicted\n";
            long long addr = block.tag << (index_size + offset_size);
            addr += newRequest.index << (offset_size);
            addr <<= 2; // byte addressable
            memWrite(addr, block_size, block.data_block);
        }
    }

    cache_block replace(vector<int> &memResp, CPUreq &newRequest)
    {
        // direct mapped
        cache_block newBlock(block_size);
        newBlock.tag = newRequest.tag;
        newBlock.data_block = memResp;
        cache_array[newRequest.index][0] = newBlock;
        return newBlock;
    }
};

vector<int> readParams(string &paramsFile)
{
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
    return params;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Incorrect Format\n";
        return 0;
    }
    string traceFile = argv[1];
    string paramsFile = "params.txt";

    // Main Memory Initialization
    for (int i = 0; i < 262144; i++)
        main_memory[i] = 4 * i;

    // Cache initialization
    vector<int> params = readParams(paramsFile);
    int cache_size = params[0];
    int ways = params[1];
    int block_size = params[2];
    string rep_policy = "LRU (Least Recently Used)";
    string write_policy = "Write Back and Write Allocate";
    Cache cache(cache_size, ways, block_size);
    int hits = 0, misses = 0, reads = 0, writes = 0, dirty_evict = 0;
    int cycles = 0, instr_count = 0;

    // Reading Traces
    vector<trace_data> traces = readTraces(traceFile);

    // Simulation
    for (auto &trace : traces)
    {
        CPUreq newRequest(trace, cache.offset_size, cache.offset_mask, cache.index_size, cache.index_mask);
        int CPUresp;
        cache_block block = cache.cache_array[newRequest.index][0];
        if (block.tag == newRequest.tag) // hit
        {
            hits++;
            if (!newRequest.read_write) // read
            {
                CPUresp = block.data_block[newRequest.offset];
                // cout << "Hit - Loaded data : " << CPUresp << "\n";
                reads++;
            }
            else
            { // write
                block.data_block[newRequest.offset] = newRequest.data;
                block.valid_bit = 1;
                block.dirty_bit = 1;
                cache.cache_array[newRequest.index][0] = block;
                // cout << "Hit - Write successful\n";
                writes++;
            }
        }
        else // miss
        {
            misses++;
            cache.evict(block, newRequest, dirty_evict);
            vector<int> memResp = memRead(trace, block_size, cache.offset_mask);
            block = cache.replace(memResp, newRequest);
            if (!newRequest.read_write) // read
            {
                CPUresp = block.data_block[newRequest.offset];
                // cout << "Miss - Loaded data : " << CPUresp << "\n";
                block.valid_bit = 1;
                block.dirty_bit = 0;
                reads++;
            }
            else // write
            {
                block.data_block[newRequest.offset] = newRequest.data;
                block.valid_bit = 1;
                block.dirty_bit = 1;
                cache.cache_array[newRequest.index][0] = block;
                // cout << "Miss - Write successful\n";
                writes++;
            }
        }
        // for (auto &i : block.data_block)
        //     cout << i << " ";
        // cout << "\n";
    }

    // Printing results
    cout << "Cache Size : " << cache_size << " KB\n";
    cout << "Associativity : " << ways << "\n";
    cout << "Block Size : " << block_size << " bytes\n";
    cout << "Replacement policy : " << rep_policy << "\n";
    cout << "Write policy : " << write_policy << "\n";
    cout << "Trace Name : " << traceFile << "\n";
    cout << "No of cache hits : " << hits << "\n";
    cout << "No of cache misses : " << misses << "\n";
    cout << "No of loads : " << reads << "\n";
    cout << "No of stores : " << writes << "\n";
    cout << "No of dirty evictions : " << dirty_evict << "\n";
    cout << "-------------------------------------\n";
    cout << "Cache Hit Rate = " << (double)hits / (hits + misses) << "\n";
    return 0;
}