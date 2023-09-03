#include "hex_dict.h"
#include "opcode.h"
#include "registers.h"
#include "r_type.h"
#include "i_type.h"
#include <iostream>
#include <fstream>
#include <vector>
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

void readAndParse(string codeLine, string &label, string &opcode, string &arg0, string &arg1, string &arg2)
{
    std::vector<string> tokens;
    std::stringstream ss(codeLine);
    string token;

    while (getline(ss, token, ' ') && token[0] != '#')
    {
        remcom(token);
        tokens.push_back(token);
    }

    if (tokens.empty() || tokens[0][0] == '#')
        return;
    if (tokens[0].back() == ':')
    {
        tokens[0].pop_back();
        label = tokens[0];
        tokens.size() >= 2 ? opcode = tokens[1] : opcode = "";
        tokens.size() >= 3 ? arg0 = tokens[2] : arg0 = "";
        tokens.size() >= 4 ? arg1 = tokens[3] : arg1 = "";
        tokens.size() >= 5 ? arg2 = tokens[4] : arg2 = "";
    }
    else
    {
        opcode = tokens[0];
        tokens.size() >= 2 ? arg0 = tokens[1] : arg0 = "";
        tokens.size() >= 3 ? arg1 = tokens[2] : arg1 = "";
        tokens.size() >= 4 ? arg2 = tokens[3] : arg2 = "";
    }
}

string binary_enc(int n, int len)
{
    string binary;
    for (int i = 0; n != 0; i++)
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

string bin_sign_imm(int n, int len)
{
    string imm = binary_enc(abs(n), len);
    if (n < 0)
    {
        int k = imm.find_last_of('1');
        for (int i = k - 1; i >= 0; i--)
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
    imm12 = bin_sign_imm(stoi(arg2), 12);
    rs1 = binary_enc(register_table[arg1], 5);
    f3 = binary_enc(i_table[opcode], 3);
    rd = binary_enc(register_table[arg0], 5);
    opcode_enc = opcode_table[opcode].first;
    return imm12 + rs1 + f3 + rd + opcode_enc;
    // work on slli, srai, shamt , encoding
}
// string s_enc(string opcode, string arg0, string arg1, string arg2) {}
// string b_enc(string opcode, string arg0, string arg1, string arg2) {}
// string j_enc(string opcode, string arg0, string arg1, string arg2) {}
// string u_enc(string opcode, string arg0, string arg1, string arg2) {}

string encoder(string label, string opcode, string arg0, string arg1, string arg2, bool choice)
{
    char type = opcode_table[opcode].second;
    string encodedString;
    // label != "" ? encodedString = label : encodedString = opcode_enc;
    switch (type)
    {
    case 'r':
        encodedString = r_enc(opcode, arg0, arg1, arg2);
        break;

    case 'i':
        encodedString = i_enc(opcode, arg0, arg1, arg2);
        break;

        // case 's':
        //     encodedString = s_enc(opcode_enc, arg0, arg1, arg2);
        //     break;

        // case 'b':
        //     encodedString = b_enc(opcode_enc, arg0, arg1, arg2);
        //     break;

        // case 'j':
        //     encodedString = j_enc(opcode_enc, arg0, arg1, arg2);
        //     break;

        // case 'u':
        //     encodedString = u_enc(opcode_enc, arg0, arg1, arg2);
        //     break;

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
    while (inString.eof() == 0)
    {
        getline(inString, codeLine);
        codeLine = rtrim(ltrim(codeLine)); // remove leading or trailing whitespaces
        string label = "", opcode = "", arg0 = "", arg1 = "", arg2 = "";
        readAndParse(codeLine, label, opcode, arg0, arg1, arg2);
        string encodedBString = encoder(label, opcode, arg0, arg1, arg2, 0);
        string encodedHString;
        encodedBString == "Incorrect opcode" ? encodedHString = encodedBString : encodedHString = "0x" + binToHex(encodedBString);
        outBString << encodedBString << "\n";
        outHString << encodedHString << "\n";
    }
    outBString.close();
    outHString.close();
    inString.close();
    return 0;
}
