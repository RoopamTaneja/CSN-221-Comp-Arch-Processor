# CSN-221 Course Project

This project was made as part of course CSN-221 : Computer Architecture and Microprocessors.<br>The project statement is in the References folder.<br>

## Contents :
- Three sample programs written in RISC-V assembly language<br>
&emsp;> 1prime.s : Check if a number stored in memory is prime or not.<br>
&emsp;> 2factorial.s : Calculate factorial of a number stored in memory location.<br>
&emsp;> 3gcd_lcm.s : Calculate GCD and LCM of two numbers stored in memory.<br>

- An assembler written in C++ (assembler.cpp) which converts RISC-V assembly codes to binary and hex encodings.

- A non-pipelined simulator written in C++ (simulator.cpp) that executes instructions encoded in binary format and writes back into a .txt file it uses as memory.


ONLY RV32I + M ext + li pseudo (25 instr)
(No floating points)

USE ABIs for registers ONLY

types of lines in asm : 
1) blank lines
2) labels - <label_name><:><space><only comments(optional)>
3) instr
PS : No instr after label in same line
4) comments can come after 2, 3, or solely comments also
5) addi r1, r2, r3 (1 whitespace only, whitespace is important, commas not so much)
6) pls no extra instructions and no wrong syntaxes

Labels support for b-type and j-type (not u-type), u can give labels or even write numeric OFFSET (not line no if numeric) (offset from PC)
pls don't start label names with digits (no labels for jalr)


In your final binary, don't have any blank lines in start, middle or end (V.IMP), or anything else except pure 0s and 1s;
### aur bhi mods hain waise
## Control word :
I only need 5 bits of 

Data file format : 
0x0000: data
0x0004: data
..
 addr and data in format above 
 <addr: data>...addr in multiples of 4 starting from 0 but any format  and any range ok
in data file : don't have any blank lines in start, middle or end (V.IMP), or anything else (use createEmp...cpp for it)

READS and Writes only decimal values from and to Data Memory

your memory addr in instr should be range : 0x00 to (i mean it's quite customisable)

otherwise single cycle non-pipelined cpu is done...it encodes and runs all programs written with those 25 instructions and gives right ans.

scalable banaya hai (esp assembler), u can easily add more RV32I instructions or pseudo in future after u get a hang of the code. U can also modify the simulator but thoda effor daalke truth_table ko samjhna and then go about it

### GUI :
Not sure if I will make the GUI. Even if I don't this interactive webpage helped me a lot with instruction encoding. Do have a look.<br>
**Webpage** : https://luplab.gitlab.io/rvcodecjs/<br> 
**Repository** : https://gitlab.com/luplab/rvcodecjs<br>

*Though adding all references used is not possible yet the ones which have been most helpful while making this project have been highlighted in the References folder. A lot of inputs and ideas have been taken from these references.*

## Cheers !!!