#include <iostream>
#include <unordered_map>

std::unordered_map<std::string, std::pair<int, int>> r_table{
    // opcode, func7, func3
    {"add", {0x00, 0x0}},
    {"sub", {0x00, 0x2}},
    {"xor", {0x00, 0x4}},
    {"or", {0x00, 0x6}},
    {"and", {0x00, 0x7}},
    {"sll", {0x00, 0x1}},
    {"sra", {0x00, 0x5}},
    // rem, mul and div
    {"rem", {0x01, 0x6}},
    {"mul", {0x01, 0x0}},
    {"div", {0x01, 0x4}},
};