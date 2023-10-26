For this project you will create a data cache simulator. The simulator you'll implement needs to work for N-way associative cache, which can be of arbitrary size (in power of 2, up to 64KB). Use the LRU (least recently used) scheme for choosing the way/block to replace in the set. For your simulation, assume a memory hierarchy that is BYTE addressable. Your simulator needs to support the following 4 parameters:

1) input_trace_file 
2) Params file : 
        -s size
        -a associativity    
        -b block_size 

The output of running your cache simulator should look like:

>cache-sim -t go_ld_trace -s 1024 -b 32 -a 2

program_name: go_ld_trace
cache_size: 1024
block_size: 32
associativity: 2
total_lds: 1500000
cache_hits: 1005939
cache_misses: 494061
cache_miss_rate:0.33

The maximum values your simulator needs to suport for each of the parameters (s,b,a) is 64K bytes for the cache size, 256 bytes for the block size, and the full associativity. 

 I recommend that you consider to have the direct mapped cache working first. 

## GRAPHS:

For the first one, a line will be drawn; the Y-axis for the graph will be the miss rate, and the X-axis for the graphs will be the size of the cache (given the fixed block size 32; fixed associativity 4). For results, you will need to run your cache simulator for "go" for cache sizes of 1KB, 2KB, 4KB, 8KB, 16 KB, 32KB, and 64KB of data.

For the second one, a line will be drawn; the Y-axis for the graph will be the miss rate, and the X-axis for the graphs will be the size of the block (given the fixed cache size 8192; fixed associativity 4). For results, you will need to run your cache simulator for "go" for block sizes of 1, 2, 4, 8, 16, 32, and 64.

For the third one, a line will be drawn; the Y-axis for the graph will be the miss rate, and the X-axis for the graphs will be the associativity (given the fixed cache size 8192; fixed block size 32). For results, you will need to run your cache simulator for "go" for associativity of 1, 2, 4, 8, 16, 32, and the fully associative. 


To support testing of your data cache simulator, you will implement a simple model of main memory. The capacity of the memory should be 1 MB (256 kilowords). At simulator start-up time, initialize the contents of each word to be the word's address. For example, the 32-bit word that starts at byte 0x00001004 should be initialized to 0x00001004. 

At the end of the trace, write all dirty blocks to the memory, so that the memory contains updated data. However, do not count them in "Number of Dirty Blocks Evicted From the Cache", since they have not really been evicted from the cache.

Your program should produce miss rates for all accesses, miss rates for loads only, and execution time for the program, in cycles. It should also show total CPI, and average memory access time (cycles per access, assuming 0 cycles for a hit and miss penalty for a miss). For execution time, assume the following: All instructions (except loads) take one cycle. A load takes one cycle plus the miss penalty. The miss penalty is 0 cycles for a cache hit and 30 cycles for a cache miss (unless specified otherwise). Loads or stores each result in a stall for miss-penalty cycles. You will simulate a write-allocate cache. In the trace shown, the first 31 instructions should take 151 cycles, assuming four cache misses and 3 cache hits for the 5 loads and 2 stores, and a 30-cycle miss penalty. You will be modeling a write-back cache, but we assume the write of a dirty line takes place mostly in the background. As such, we assume an extra 2-cycle delay to write a dirty line to a write buffer. So, using the parameters from above, a load or store miss that would evict a clean line takes 30 cycles, and if evicting a dirty line takes 32 cycles. Cache replacement policy is always LRU for associative caches. If useful, assume a 2 GHz processor. 

cache_sim : traces1, traces2
raw_cache_sim : all