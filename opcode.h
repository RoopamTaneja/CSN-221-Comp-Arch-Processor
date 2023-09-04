#include <iostream>
#include <unordered_map>

std::unordered_map<std::string, std::pair<std::string, char>> opcode_table = {
    // r-type
    {"add", {"0110011", 'r'}},
    {"sub", {"0110011", 'r'}},
    {"xor", {"0110011", 'r'}},
    {"or", {"0110011", 'r'}},
    {"and", {"0110011", 'r'}},
    {"sll", {"0110011", 'r'}},
    {"sra", {"0110011", 'r'}},
    // rem, mul and div
    {"rem", {"0110011", 'r'}},
    {"mul", {"0110011", 'r'}},
    {"div", {"0110011", 'r'}},
    // i-type alu
    {"addi", {"0010011", 'i'}},
    {"xori", {"0010011", 'i'}},
    {"ori", {"0010011", 'i'}},
    {"andi", {"0010011", 'i'}},
    {"slli", {"0010011", 'i'}},
    {"srai", {"0010011", 'i'}},
    // i-type load
    {"lw", {"0000011", 'i'}},
    // s-type
    {"sw", {"0100011", 's'}},
    // b-type
    {"beq", {"1100011", 'b'}},
    {"blt", {"1100011", 'b'}},
    // jal
    {"jal", {"1101111", 'j'}},
    // jalr
    {"jalr", {"1100111", 'i'}},
    // lui
    {"lui", {"0110111", 'u'}},
    // auipc
    {"auipc", {"0010111", 'u'}},
    // pseudo
    {"li", {"xxxxxxx", 'p'}},
    // 25 instructions to begin with
};
