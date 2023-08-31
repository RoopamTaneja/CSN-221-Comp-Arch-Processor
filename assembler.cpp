#include "opcode.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
using std::cout, std::cin, std::string;

void newReadAndParse(string codeLine, string &label, string &opcode, string &arg0, string &arg1, string &arg2)
{
    std::vector<string> tokens;
    std::stringstream ss(codeLine);
    std::string token;

    while (getline(ss, token, ' ') && token[0] != '#')
        tokens.push_back(token);

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
string encoder(string label, string opcode, string arg0, string arg1, string arg2)
{
    string opcode_enc = opcode_table[opcode];
    string encodedString = opcode_enc;
    return encodedString;
}
int isNumber(char *string)
{
    /* return 1 if string is a number */
    int i;
    return ((sscanf(string, "%d", &i)) == 1);
}
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Incorrect Format\n";
        return 0;
    }
    string inFileString = argv[1];
    string outFileString = argv[2];
    string codeLine;
    std::ifstream inString(inFileString);
    if (inString.bad())
    {
        cout << "Error in opening : " << inFileString << "\n";
        return 0;
    }
    std::ofstream outString(outFileString, std::ios::app);
    if (outString.bad())
    {
        cout << "Error in opening : " << outFileString << "\n";
        return 0;
    }
    while (inString.eof() == 0)
    {
        getline(inString, codeLine);
        string label = "", opcode = "", arg0 = "", arg1 = "", arg2 = "";
        newReadAndParse(codeLine, label, opcode, arg0, arg1, arg2);
        string encodedString = encoder(label, opcode, arg0, arg1, arg2);
        outString << encodedString << "\n";
    }
    outString.close();
    inString.close();
    return 0;
}
