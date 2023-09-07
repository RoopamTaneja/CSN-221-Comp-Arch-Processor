#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <sstream>
using std::cout, std::cin, std::string;

int dec_sign_imm(string s)
{
    int dec_val = 0, base = 1;
    bool neg = (s[0] == '1');
    if (neg)
    {
        int k = s.find_last_of('1');
        for (int i = k - 1; i >= 0; i--)
            s[i] == '0' ? s[i] = '1' : s[i] = '0';
    }
    int len = s.length();
    for (int i = len - 1; i >= 0; i--)
    {
        if (s[i] == '1')
            dec_val += base;
        base = base * 2;
    }
    if (neg)
        return -1 * dec_val;
    return dec_val;
}

class Controller
{
public:
    bool op1Sel, op2Sel;
    string immSel, ALUop;
    bool regRead, regWrite;
    bool memRead, memWrite, mem2Reg;
    string branch, jump;

    void setImmSel_n_ALUop(string op5)
    {
        if (op5 == "01100")
            immSel = "000", ALUop = "00";
        else if (op5 == "00100")
            immSel = "001", ALUop = "01";
        else if (op5 == "00000" || op5 == "11001")
            immSel = "001", ALUop = "10";
        else if (op5 == "01000")
            immSel = "010", ALUop = "10";
        else if (op5 == "11000")
            immSel = "011", ALUop = "11";
        else if (op5 == "11011")
            immSel = "100", ALUop = "10";
        else
            immSel = "101", ALUop = "10";
    }

    Controller(string op5, string f3)
    {
        if (op5 == "11011" || op5 == "11001" || op5 == "00101")
            op1Sel = 1;
        else
            op1Sel = 0;

        if (op5 == "01100" || op5 == "11000")
            op2Sel = 0;
        else
            op2Sel = 1;

        setImmSel_n_ALUop(op5);

        if (op5 == "11011" || op5 == "01101" || op5 == "00101")
            regRead = 0;
        else
            regRead = 1;

        if (op5 == "01000" || op5 == "11000")
            regWrite = 0;
        else
            regWrite = 1;

        if (op5 == "00000")
            memRead = 1, mem2Reg = 1;
        else
            memRead = 0, mem2Reg = 0;

        if (op5 == "01000")
            memWrite = 1;
        else
            memWrite = 0;

        if (op5 == "11000")
        {
            if (f3 == "000")
                branch = "01";
            else if (f3 == "100")
                branch = "10";
        }
        else
            branch = "00";

        if (op5 == "11011")
            jump = "01";
        else if (op5 == "11001")
            jump = "10";
        else
            jump = "00";
    }

    void checkCtrlWord()
    {
        cout << op1Sel << op2Sel << immSel << ALUop << regRead << regWrite << memRead
             << memWrite << mem2Reg << branch << jump << "\n";
    }
};

int immGen(const string instr, string immSel)
{
    int imm;
    if (immSel == "000") // no imm needed
        imm = INT32_MIN;
    else if (immSel == "001") // i-type imm
        imm = dec_sign_imm(instr.substr(0, 12));
    else if (immSel == "010")
        imm = dec_sign_imm(instr.substr(0, 7) + instr.substr(20, 5));
    else if (immSel == "011")
    {
        string offset = "";
        offset += instr[0] + instr[24];
        offset += instr.substr(1, 6) + instr.substr(20, 4);
        imm = dec_sign_imm(offset);
        imm <<= 1;
    }
    else if (immSel == "100")
    {
        string offset = "";
        offset += instr[0];
        offset += instr.substr(12, 8);
        offset += instr[11];
        offset += instr.substr(1, 10);
        imm = dec_sign_imm(offset);
        imm <<= 1;
    }
    else if (immSel == "101")
    {
        imm = dec_sign_imm(instr.substr(0, 20));
        imm <<= 12;
    }
    return imm;
}

string ALUcontrol(string ALUop, string f3, char f7)
{
    if (ALUop == "10")
        return "0000";
    if (ALUop == "11")
        return "0001";
    if (ALUop == "00") // r-type
    {
        if (f3 == "000" && f7 == '0')
            return "0000";
        if (f3 == "010" && f7 == '0')
            return "0001";
        if (f3 == "100" && f7 == '0')
            return "0101";
        if (f3 == "110" && f7 == '0')
            return "0110";
        if (f3 == "111" && f7 == '0')
            return "0111";
        if (f3 == "001" && f7 == '0')
            return "1000";
        if (f3 == "101" && f7 == '0')
            return "1001";
        if (f3 == "110" && f7 == '1')
            return "0100";
        if (f3 == "000" && f7 == '1')
            return "0010";
        if (f3 == "100" && f7 == '1')
            return "0011";
    }
    // i-type
    if (f3 == "000")
        return "0000";
    if (f3 == "100")
        return "0101";
    if (f3 == "110")
        return "0110";
    if (f3 == "111")
        return "0111";
    if (f3 == "001")
        return "1000";
    return "1001";
}

class ALU
{
public:
    int ALUresult;
    bool zeroFlag;
    bool LTflag;
    ALU(string ALUsel, int rs1, int rs2)
    {
        if (ALUsel == "0000")
            ALUresult = rs1 + rs2;
        else if (ALUsel == "0001")
            ALUresult = rs1 - rs2;
        else if (ALUsel == "0010")
            ALUresult = rs1 * rs2;
        else if (ALUsel == "0011")
            ALUresult = rs1 / rs2;
        else if (ALUsel == "0100")
            ALUresult = rs1 % rs2;
        else if (ALUsel == "0101")
            ALUresult = rs1 ^ rs2;
        else if (ALUsel == "0110")
            ALUresult = rs1 | rs2;
        else if (ALUsel == "0111")
            ALUresult = rs1 & rs2;
        else if (ALUsel == "1000")
            ALUresult = rs1 << rs2;
        else if (ALUsel == "1001")
            ALUresult = rs1 >> rs2;

        zeroFlag = (ALUresult == 0);
        LTflag = (ALUresult < 0);
    }
};

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Incorrect Format\n";
        return 0;
    }
    string instrFile = argv[1];
    string dataFile = argv[2];
    string instrLine, dataLine;
    // Loading the data into program DM
    std::vector<int> DM;
    std::ifstream inData(dataFile);
    if (inData.bad())
    {
        cout << "Error in opening : " << dataFile << "\n";
        return 0;
    }
    while (getline(inData, dataLine))
    {
        std::vector<string> tokens;
        std::stringstream ss(dataLine);
        string token;
        while (getline(ss, token, ' '))
            tokens.push_back(token);

        DM.push_back(stoi(tokens[1]));
    }
    inData.close();
    // Loading the instr into program IM
    std::vector<string> IM;
    std::ifstream inInstr(instrFile);
    if (inInstr.bad())
    {
        cout << "Error in opening : " << instrFile << "\n";
        return 0;
    }
    while (getline(inInstr, instrLine))
        IM.push_back(instrLine);

    inInstr.close();
    int numInstr = IM.size();
    std::vector<int> regFile(32, 0);

    // Executing the instr
    int PC = 0;
    while (PC != numInstr * 4)
    {
        // stage 1
        string instr = IM[PC / 4];

        // stage 2
        string op5 = instr.substr(25, 5);
        string f3 = instr.substr(17, 3);
        char f7 = instr[6];
        int rsl1 = stoi(instr.substr(12, 5), NULL, 2);
        int rsl2 = stoi(instr.substr(7, 5), NULL, 2);
        int rdl = stoi(instr.substr(20, 5), NULL, 2);
        Controller CW(op5, f3);
        int imm = immGen(instr, CW.immSel);
        int rs1 = 0, rs2 = 0;
        if (CW.regRead)
        {
            rs1 = regFile[rsl1];
            rs2 = regFile[rsl2];
        }

        // stage 3
        if (CW.op1Sel)
            rs1 = PC;
        if (CW.op2Sel)
            rs2 = imm;
        if (CW.jump != "00")
            rs2 = 4;
        string ALUsel = ALUcontrol(CW.ALUop, f3, f7);
        ALU aluRes(ALUsel, rs1, rs2);

        int JPC;
        if (CW.jump == "01") // jal
            JPC = PC + imm;
        else if (CW.jump == "10") // jalr
            JPC = rs1 + imm;

        int BPC = PC + imm;
        if (CW.jump != "00")
            PC = JPC;
        else if ((CW.branch == "01" && aluRes.zeroFlag) || CW.branch == "10" && aluRes.LTflag)
            PC = BPC;
        else
            PC = PC + 4;

        // stage 4
        int LDres;
        if (CW.memWrite && CW.regRead)
        {
            rs2 = regFile[rsl2];
            DM[aluRes.ALUresult / 4] = rs2;
        }
        if (CW.memRead)
            LDres = DM[aluRes.ALUresult / 4];

        // stage 5
        if (CW.regWrite)
        {
            if (CW.mem2Reg)
                regFile[rdl] = LDres;
            else
                regFile[rdl] = aluRes.ALUresult;
        }
        regFile[0] = 0;
    }
    
    // Printing back the data from DM
    std::ofstream outData(dataFile, std::ios::trunc);
    if (outData.bad())
    {
        cout << "Error in opening : " << dataFile << "\n";
        return 0;
    }
    int addr = 0;
    for (auto &d : DM)
    {
        outData << "0x" << std::setw(4) << std::setfill('0') << std::uppercase << std::hex << addr << ": " << std::dec << d << "\n";
        addr += 4;
    }
    outData.close();
    return 0;
}