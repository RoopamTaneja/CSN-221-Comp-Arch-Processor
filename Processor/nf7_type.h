#include <iostream>
#include <unordered_map>

std::unordered_map<std::string, int> nf7_table{
    // opcode, func3
    {"addi", 0x0},
    {"xori", 0x4},
    {"ori", 0x6},
    {"andi", 0x7},
    {"slli", 0x1},
    {"srai", 0x5},
    {"lw", 0x2},
    {"sw", 0x2},
    {"beq", 0x0},
    {"blt", 0x4},
    {"jalr", 0x0},
};