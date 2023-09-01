#include <iostream>
#include <unordered_map>

std::unordered_map<std::string, std::string> opcode_table = {
    // r-type
    {"add", "0110011"},
    {"sub", "0110011"},
    {"xor", "0110011"},
    {"or", "0110011"},
    {"and", "0110011"},
    {"sll", "0110011"},
    {"srl", "0110011"},
    {"sra", "0110011"},
    // rem, mul and div
    {"rem", "0110011"},
    {"mul", "0110011"},
    {"div", "0110011"},
    // i-type alu
    {"addi", "0010011"},
    {"xori", "0010011"},
    {"ori", "0010011"},
    {"andi", "0010011"},
    {"slli", "0010011"},
    {"srli", "0010011"},
    {"srai", "0010011"},
    // i-type load
    {"lb", "0000011"},
    {"lw", "0000011"},
    // s-type
    {"sb", "0100011"},
    {"sw", "0100011"},
    // b-type
    {"beq", "1100011"},
    {"bne", "1100011"},
    {"blt", "1100011"},
    // jal
    {"jal", "1101111"},
    // jalr
    {"jalr", "1100111"},
    // lui
    {"lui", "0110111"},
    // auipc
    {"auipc", "0010111"},
    // ecall
    {"ecall", "1110011"},
    // 30 instructions to begin with
};
