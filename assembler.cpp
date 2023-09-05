#include "hex_dict.h"
#include "opcode.h"
#include "registers.h"
#include "r_type.h"
#include "nf7_type.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
using std::cout, std::cin, std::string;

const string WHITESPACE = " \n\r\t\f\v";
string ltrim(const string &s)
{
    int start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

string rtrim(const string &s)
{
    if (s == "")
        return s;
    int end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

void remcom(string &s)
{
    int size = s.size();
    if (s.front() == ',')
        s = s.substr(1, size - 1);
    if (s.back() == ',')
        s.pop_back();
}

std::map<string, int> parseLabels(string inFileString)
{
    string codeLine;
    std::ifstream inLabels(inFileString);
    int addr = 0;
    std::map<string, int> labelAddr;
    while (inLabels.eof() == 0)
    {
        getline(inLabels, codeLine);
        codeLine = rtrim(ltrim(codeLine));
        if (codeLine == "")
            continue;
        std::stringstream ss(codeLine);
        string token;
        getline(ss, token, ' ');
        remcom(token);
        if (token.front() == '#')
            continue;
        else if (token.back() == ':')
            labelAddr[token] = addr;
        else if (token == "li") // be careful for li
            addr += 8;
        else
            addr += 4;
    }
    inLabels.close();
    return labelAddr;
}

void parseInstr(string codeLine, string &opcode, string &arg0, string &arg1, string &arg2)
{
    std::vector<string> tokens;
    std::stringstream ss(codeLine);
    string token;
    while (getline(ss, token, ' '))
    {
        remcom(token);
        if (token.front() == '#')
            break;
        tokens.push_back(token);
    }
    opcode = tokens[0];
    tokens.size() >= 2 ? arg0 = tokens[1] : arg0 = "";
    tokens.size() >= 3 ? arg1 = tokens[2] : arg1 = "";
    tokens.size() >= 4 ? arg2 = tokens[3] : arg2 = "";
    if (arg1.back() == ')')
    {
        arg2 = arg1;
        int cnt = 0, i = arg2.length() - 2;
        while (arg2[i] != '(') // handling bracket arguments
            cnt++, i--;
        arg1 = arg2.substr(i + 1, cnt);
        arg2 = arg2.substr(0, i);
    }
}

string binary_enc(long long n, int len)
{
    string binary;
    while (n != 0)
    {
        binary.push_back(n % 2 + '0');
        n = n / 2;
    }
    while (binary.length() < len)
        binary.push_back('0');
    binary = binary.substr(0, len);
    reverse(binary.begin(), binary.end());
    return binary;
}

string bin_sign_imm(long long n, int len)
{
    string imm = binary_enc(abs(n), len);
    if (n < 0)
    {
        long long k = imm.find_last_of('1');
        for (long long i = k - 1; i >= 0; i--)
            imm[i] == '0' ? imm[i] = '1' : imm[i] = '0';
    }
    return imm;
}

string binToHex(string bin)
{
    string hex;
    bin = string(bin.length() % 4 ? 4 - bin.length() % 4 : 0, '0') + bin;
    for (int i = 0; i < bin.length(); i += 4)
    {
        string group = bin.substr(i, 4);
        hex += hex_dict[group];
    }
    return hex;
}

string r_enc(string opcode, string arg0, string arg1, string arg2)
{
    string f7, rs2, rs1, f3, rd, opcode_enc;
    f7 = binary_enc(r_table[opcode].first, 7);
    rs2 = binary_enc(register_table[arg2], 5);
    rs1 = binary_enc(register_table[arg1], 5);
    f3 = binary_enc(r_table[opcode].second, 3);
    rd = binary_enc(register_table[arg0], 5);
    opcode_enc = opcode_table[opcode].first;
    return f7 + rs2 + rs1 + f3 + rd + opcode_enc;
}

string i_enc(string opcode, string arg0, string arg1, string arg2)
{
    string imm12, rs1, f3, rd, opcode_enc;
    // assuming shamt entered is from 0-31 and since prefix decided = 7 zeroes,
    // nothing special needs to be done
    if (arg2[1] == 'x')
        imm12 = bin_sign_imm(stoll(arg2, NULL, 16), 12);
    else
        imm12 = bin_sign_imm(stoll(arg2), 12);
    rs1 = binary_enc(register_table[arg1], 5);
    f3 = binary_enc(nf7_table[opcode], 3);
    rd = binary_enc(register_table[arg0], 5);
    opcode_enc = opcode_table[opcode].first;
    return imm12 + rs1 + f3 + rd + opcode_enc;
}

string s_enc(string opcode, string arg0, string arg1, string arg2)
{
    string imm12, imm1, imm2, rs2, rs1, f3, opcode_enc;
    if (arg2[1] == 'x')
        imm12 = bin_sign_imm(stoll(arg2, NULL, 16), 12);
    else
        imm12 = bin_sign_imm(stoll(arg2), 12);
    imm1 = imm12.substr(0, 7);
    imm2 = imm12.substr(7, 5);
    rs2 = binary_enc(register_table[arg0], 5);
    rs1 = binary_enc(register_table[arg1], 5);
    f3 = binary_enc(nf7_table[opcode], 3);
    opcode_enc = opcode_table[opcode].first;
    return imm1 + rs2 + rs1 + f3 + imm2 + opcode_enc;
}

string b_enc(string opcode, string arg0, string arg1, string arg2)
{
    string imm12, imm1, imm2, rs2, rs1, f3, opcode_enc;
    if (arg2[1] == 'x')
        imm12 = bin_sign_imm(stoll(arg2, NULL, 16), 13);
    else
        imm12 = bin_sign_imm(stoll(arg2), 13);
    imm12.pop_back();
    imm1 = imm12[0] + imm12.substr(2, 6);
    imm2 = imm12.substr(8, 4) + imm12[1];
    rs2 = binary_enc(register_table[arg1], 5);
    rs1 = binary_enc(register_table[arg0], 5);
    f3 = binary_enc(nf7_table[opcode], 3);
    opcode_enc = opcode_table[opcode].first;
    return imm1 + rs2 + rs1 + f3 + imm2 + opcode_enc;
}

string j_enc(string opcode, string arg0, string arg1)
{
    string imm20, imm, rd, opcode_enc;
    if (arg1[1] == 'x')
        imm20 = bin_sign_imm(stoll(arg1, NULL, 16), 21);
    else
        imm20 = bin_sign_imm(stoll(arg1), 21);
    imm20.pop_back();
    imm = imm20[0] + imm20.substr(10, 10) + imm20[9] + imm20.substr(1, 8);
    rd = binary_enc(register_table[arg0], 5);
    opcode_enc = opcode_table[opcode].first;
    return imm + rd + opcode_enc;
}

string u_enc(string opcode, string arg0, string arg1)
{
    string imm20, rd, opcode_enc;
    if (arg1[1] == 'x')
        imm20 = bin_sign_imm(stoll(arg1, NULL, 16), 20);
    else
        imm20 = bin_sign_imm(stoll(arg1), 20);
    rd = binary_enc(register_table[arg0], 5);
    opcode_enc = opcode_table[opcode].first;
    return imm20 + rd + opcode_enc;
}

string li_enc(string arg0, string arg1)
{
    string instr1, instr2;
    long long imm1, imm2;
    if (arg1[1] == 'x')
        imm1 = stoll(arg1, NULL, 16);
    else
        imm1 = stoll(arg1);
    imm2 = (imm1 + 0x800) >> 12;
    arg1 = std::to_string(imm2);
    instr1 = u_enc("lui", arg0, arg1);
    arg1 = std::to_string(imm1 & 0xfff);
    instr2 = i_enc("addi", arg0, arg0, arg1);
    return instr1 + "\n" + instr2;
}

string p_enc(string opcode, string arg0, string arg1, string arg2)
{
    string opcode_enc = opcode_table[opcode].first;
    if (opcode_enc == "xxxxxxx")
        return li_enc(arg0, arg1);
    return "";
}

string encoder(string opcode, string arg0, string arg1, string arg2)
{
    char type = opcode_table[opcode].second;
    string encodedString;
    switch (type)
    {
    case 'r':
        encodedString = r_enc(opcode, arg0, arg1, arg2);
        break;

    case 'i':
        encodedString = i_enc(opcode, arg0, arg1, arg2);
        break;

    case 's':
        encodedString = s_enc(opcode, arg0, arg1, arg2);
        break;

    case 'b':
        encodedString = b_enc(opcode, arg0, arg1, arg2);
        break;

    case 'j':
        encodedString = j_enc(opcode, arg0, arg1);
        break;

    case 'u':
        encodedString = u_enc(opcode, arg0, arg1);
        break;

    case 'p':
        encodedString = p_enc(opcode, arg0, arg1, arg2);
        break;

    default:
        encodedString = "Incorrect opcode";
        break;
    }
    return encodedString;
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        cout << "Incorrect Format\n";
        return 0;
    }
    string inFileString = argv[1];
    string outBinString = argv[2];
    string outHexString = argv[3];
    string codeLine;
    std::ifstream inString(inFileString);
    if (inString.bad())
    {
        cout << "Error in opening : " << inFileString << "\n";
        return 0;
    }
    std::ofstream outBString(outBinString, std::ios::app);
    std::ofstream outHString(outHexString, std::ios::app);
    if (outBString.bad())
    {
        cout << "Error in opening : " << outBinString << "\n";
        return 0;
    }
    if (outHString.bad())
    {
        cout << "Error in opening : " << outHexString << "\n";
        return 0;
    }
    std::map<string, int> labelAddr = parseLabels(inFileString);
    // for (auto &it : labelAddr)
    // {
    //     cout << it.first << " " << it.second << "\n";
    // }
    while (inString.eof() == 0)
    {
        getline(inString, codeLine);
        codeLine = rtrim(ltrim(codeLine)); // remove leading or trailing whitespaces
        if (codeLine == "")
            continue; // to handle blank lines
        std::stringstream ss(codeLine);
        string token;
        getline(ss, token, ' ');
        remcom(token);
        if (token.front() == '#' || token.back() == ':')
            continue;
        string opcode = "", arg0 = "", arg1 = "", arg2 = "";
        parseInstr(codeLine, opcode, arg0, arg1, arg2);
        string encodedBString = encoder(opcode, arg0, arg1, arg2);
        string encodedHString;
        if (encodedBString == "Incorrect opcode")
            encodedHString = encodedBString;
        else if (encodedBString.find('\n') != string::npos)
        {
            std::vector<string> tokens;
            std::stringstream ss(encodedBString);
            string token;
            while (getline(ss, token, '\n'))
                tokens.push_back(token);
            for (int i = 0; i < tokens.size() - 1; i++)
                encodedHString = "0x" + binToHex(tokens[i]) + "\n";
            encodedHString += "0x" + binToHex(tokens.back());
        }
        else
            encodedHString = "0x" + binToHex(encodedBString);
        outBString << encodedBString << "\n";
        outHString << encodedHString << "\n";
    }
    outBString.close();
    outHString.close();
    inString.close();
    return 0;
}
