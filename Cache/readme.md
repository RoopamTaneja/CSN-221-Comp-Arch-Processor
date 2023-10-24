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

  For the first one, a line will be drawn; the Y-axis for the graph will be the miss rate, and the X-axis for the graphs will be the size of the cache (given the fixed block size 32; fixed associativity 4). For results, you will need to run your cache simulator for "go" for cache sizes of 1KB, 2KB, 4KB, 8KB, 16 KB, 32KB, and 64KB of data.

For the second one, a line will be drawn; the Y-axis for the graph will be the miss rate, and the X-axis for the graphs will be the size of the block (given the fixed cache size 8192; fixed associativity 4). For results, you will need to run your cache simulator for "go" for block sizes of 1, 2, 4, 8, 16, 32, and 64.

For the third one, a line will be drawn; the Y-axis for the graph will be the miss rate, and the X-axis for the graphs will be the associativity (given the fixed cache size 8192; fixed block size 32). For results, you will need to run your cache simulator for "go" for associativity of 1, 2, 4, 8, 16, 32, and the fully associative. 


To support testing of your data cache simulator, you will implement a simple model of main memory. The capacity of the memory should be 1 MB (256 kilowords). At simulator start-up time, initialize the contents of each word to be the word's address. For example, the 32-bit word that starts at byte 0x00001004 should be initialized to 0x00001004. 

At the end of the trace, write all dirty blocks to the memory, so that the memory contains updated data. However, do not count them in "Number of Dirty Blocks Evicted From the Cache", since they have not really been evicted from the cache.




Below are sample results for the program go:

cache-sim -t go_ld_trace -s 1024 -b 32 -a 4
program_name: go_ld_trace
cache_size: 1024
block_size: 32
associativity: 4
total_lds: 1500000
cache_hits: 1036092
cache_misses: 463908
cache_miss_rate: 0.31

cache-sim -t go_ld_trace -s 2048 -b 32 -a 4
program_name: go_ld_trace
cache_size: 2048
block_size: 32
associativity: 4
total_lds: 1500000
cache_hits: 1212014
cache_misses: 287986
cache_miss_rate: 0.192

cache-sim -t go_ld_trace -s 2048 -b 32 -a 64
program_name: go_ld_trace
cache_size: 2048
block_size: 32
associativity: 64
total_lds: 1500000
cache_hits: 1232218
cache_misses: 267782
cache_miss_rate: 0.18

cache-sim -t go_ld_trace -s 8192 -b 32 -a 256
program_name: go_ld_trace
cache_size: 8192
block_size: 32
associativity: 256 (fully associative)
total_lds: 1500000
cache_hits: 1476159
cache_misses: 23841
cache_miss_rate: 0.0159

cache-sim -t go_ld_trace -s 4096 -b 32 -a 1
program_name: go_ld_trace
cache_size: 4096
block_size: 32
associativity: 1 (Direct Mapped Cache)
total_lds: 1500000
cache_hits: 1252626
cache_misses: 247374
cache_miss_rate: 0.1649


