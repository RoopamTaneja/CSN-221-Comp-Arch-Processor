#include <iostream>
#include <unordered_map>

std::unordered_map<std::string, int> i_table{
    // opcode, func3
    {"addi", 0x0},
    {"xori", 0x4},
    {"ori", 0x6},
    {"andi", 0x7},
    {"slli", 0x1},
    {"srai", 0x5},
};