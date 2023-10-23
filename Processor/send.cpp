// 5-stage pipelined RISC-V processor simulator with operand forwarding

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
        if (op5 == "00101")
            op1Sel = 1;
        else
            op1Sel = 0;

        if (op5 == "01100" || op5 == "11000" || op5 == "11011" || op5 == "11001")
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
    else if (immSel == "010") // s-type imm
        imm = dec_sign_imm(instr.substr(0, 7) + instr.substr(20, 5));
    else if (immSel == "011") // b-type imm
    {
        string offset = "";
        offset += instr[0] + instr[24];
        offset += instr.substr(1, 6) + instr.substr(20, 4);
        imm = dec_sign_imm(offset);
        imm <<= 1;
    }
    else if (immSel == "100") // j type imm
    {
        string offset = "";
        offset += instr[0];
        offset += instr.substr(12, 8);
        offset += instr[11];
        offset += instr.substr(1, 10);
        imm = dec_sign_imm(offset);
        imm <<= 1;
    }
    else if (immSel == "101") // u type imm
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
    ALU(){};
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

class pipelineRegister
{
public:
    bool valid = false, stall = false;
};

class pc : public pipelineRegister
{
public:
    int IA;
};

class ifid : public pipelineRegister
{
public:
    int instr_PC;
    string instr_reg;
};

class idex : public pipelineRegister
{
public:
    int instr_PC;
    int imm;
    Controller CW;
    int rs1, rs2;
    int rsl1, rsl2, rdl;
    string ALUsel;
    idex() : CW("00000", "xxx") {}
};

class exmo : public pipelineRegister
{
public:
    int instr_PC;
    Controller CW;
    int ALUres;
    int rsl2, rdl;
    exmo() : CW("00000", "xxx") {}
};

class mowb : public pipelineRegister
{
public:
    int instr_PC;
    Controller CW;
    int ALUres, LDres;
    int rdl;
    mowb() : CW("00000", "xxx") {}
};

bool load_use_hazard(idex &IDEX, exmo &EXMO)
{
    string check = IDEX.CW.immSel;
    if (!EXMO.CW.memRead)
        return false; // producer must be a load
    if (check == "100" || check == "101")
        return false; // no hazard for j-type, u-type
    if (EXMO.rdl == IDEX.rsl1)
        return true;
    if ((check == "000" || check == "011") && (EXMO.rdl == IDEX.rsl2))
        return true; // rsl2 dep for r-type, b-type

    return false;
}

bool rs1_hazard(idex &IDEX, exmo &EXMO, mowb &MOWB, bool choice)
{
    bool hazard_flag = false;
    if (!choice)
    {
        if (EXMO.CW.immSel == "010" || EXMO.CW.immSel == "011")
            return false; // no hazard if producer is b-type, s-type
        if (EXMO.CW.memRead)
            return false; // no hazard here if producer is load
        if (IDEX.CW.immSel == "100" || IDEX.CW.immSel == "101")
            return false; // no hazard if consumer is j-type, u-type
        if (EXMO.rdl == IDEX.rsl1)
            hazard_flag = true;
    }
    else
    {
        if (MOWB.CW.immSel == "010" || MOWB.CW.immSel == "011")
            return false; // no hazard if producer is b-type, s-type
        if (IDEX.CW.immSel == "100" || IDEX.CW.immSel == "101")
            return false; // no hazard if consumer is j-type, u-type
        if (MOWB.rdl == IDEX.rsl1)
            hazard_flag = true;
    }
    return hazard_flag;
}

bool rs2_hazard(idex &IDEX, exmo &EXMO, mowb &MOWB, bool choice)
{
    bool hazard_flag = false;
    if (!choice)
    {
        if (EXMO.CW.immSel == "010" || EXMO.CW.immSel == "011")
            return false; // no hazard if producer is b-type, s-type
        if (EXMO.CW.memRead)
            return false; // no hazard here if producer is load
        if (IDEX.CW.immSel == "001" || IDEX.CW.immSel == "100" || IDEX.CW.immSel == "101" || IDEX.CW.immSel == "010")
            return false; // no hazard if consumer is j-type, u-type, i-type, load, s-type
        if (EXMO.rdl == IDEX.rsl2)
            hazard_flag = true;
    }
    else
    {
        string check = IDEX.CW.immSel;
        if (MOWB.CW.immSel == "010" || MOWB.CW.immSel == "011")
            return false; // no hazard if producer is b-type, s-type
        if (check == "001" || check == "100" || check == "101" || check == "010")
            return false; // no hazard if consumer is j-type, u-type, i-type, load, s-type
        if (MOWB.rdl == IDEX.rsl2)
            hazard_flag = true;
    }
    return hazard_flag;
}

int forwarder_ex(idex &IDEX, exmo &EXMO, mowb &MOWB, int &t1_forwards, int &t2_forwards, bool choice)
{ // 0 - EXMO forward, 1 - MOWB forward
    int operand;
    if (!choice)
    {
        if (rs1_hazard(IDEX, EXMO, MOWB, 0))
        { // prefer EXMO forward
            operand = EXMO.ALUres;
            t1_forwards++;
        }
        else if (rs1_hazard(IDEX, EXMO, MOWB, 1))
        {
            if (MOWB.CW.memRead)
                operand = MOWB.LDres;
            else
                operand = MOWB.ALUres;
            t2_forwards++;
        }
        else
            operand = IDEX.rs1;
    }
    else
    {
        if (rs2_hazard(IDEX, EXMO, MOWB, 0))
        { // prefer EXMO forward
            operand = EXMO.ALUres;
            t1_forwards++;
        }
        else if (rs2_hazard(IDEX, EXMO, MOWB, 1))
        {
            if (MOWB.CW.memRead)
                operand = MOWB.LDres;
            else
                operand = MOWB.ALUres;
            t2_forwards++;
        }
        else
            operand = IDEX.rs2;
    }
    return operand;
}

bool sw_rs2_hazard(exmo &EXMO, mowb &MOWB)
{
    bool hazard_flag = false;
    if (MOWB.CW.immSel == "010" || MOWB.CW.immSel == "011")
        return false; // no hazard if producer is b-type, s-type
    if (EXMO.CW.immSel != "010")
        return false; // no hazard if consumer is not s-type
    if (MOWB.rdl == EXMO.rsl2)
        hazard_flag = true;

    return hazard_flag;
}

int forwarder_mo(exmo &EXMO, mowb &MOWB, int &t3_forwards)
{
    if (sw_rs2_hazard(EXMO, MOWB))
    {
        t3_forwards++;
        if (MOWB.CW.memRead)
            return MOWB.LDres;
        return MOWB.ALUres;
    }
    return EXMO.rdl;
}

void instr_fetch(const std::vector<string> &IM, pc &PC, ifid &IFID)
{
    if (!PC.valid)
    {
        if (!IFID.stall)
            IFID.valid = false;
        return;
    }
    if (IFID.stall)
        return;

    // IFID writing and PC update
    IFID.valid = true;
}

void instr_decode(int regFile[], ifid &IFID, idex &IDEX, int &decode_count, int &reg_read_count)
{
    if (!IFID.valid)
    {
        if (!IDEX.stall)
            IDEX.valid = false;
        return;
    }
    if (IDEX.stall)
    {
        IFID.stall = true;
        return;
    }
    // IDEX writing

    IDEX.rsl1 = stoi(IFID.instr_reg.substr(12, 5), NULL, 2);
    IDEX.rsl2 = stoi(IFID.instr_reg.substr(7, 5), NULL, 2);
    IDEX.rs1 = 0, IDEX.rs2 = 0;
    if (IDEX.CW.op1Sel) // auipc
        IDEX.rs1 = IDEX.instr_PC;
    else
        IDEX.rs1 = regFile[IDEX.rsl1];

    if (IDEX.CW.op2Sel)
        IDEX.rs2 = IDEX.imm;
    else
        IDEX.rs2 = regFile[IDEX.rsl2];

    if ((IDEX.CW.immSel == "010"))
        IDEX.rdl = regFile[IDEX.rsl2]; // forwarding rs2 for sw
    else
        IDEX.rdl = stoi(IFID.instr_reg.substr(20, 5), NULL, 2);

    IFID.stall = false;
    IDEX.valid = true;
}

void instr_execute(idex &IDEX, exmo &EXMO, ifid &IFID, mowb &old_MOWB, pc &PC, int &alu_count, int &total_stalls, int &total_flushes, int &t1_forwards, int &t2_forwards)
{
    if (!IDEX.valid)
    {
        if (!EXMO.stall)
            EXMO.valid = false;
        return;
    }
    if (!EXMO.stall && load_use_hazard(IDEX, EXMO)) // stalling for only one cycle
    {
        if (IDEX.CW.jump == "00")
        { // 0 - inp1; 1 - inp2
            IDEX.rs1 = forwarder_ex(IDEX, EXMO, old_MOWB, t1_forwards, t2_forwards, 0);
            IDEX.rs2 = forwarder_ex(IDEX, EXMO, old_MOWB, t1_forwards, t2_forwards, 1);
        }
        total_stalls++;
        IDEX.stall = true;
        EXMO.valid = false;
        EXMO.stall = true;
        return;
    }

    int ALU_inp1, ALU_inp2;
    if (IDEX.CW.jump != "00")
        ALU_inp1 = IDEX.instr_PC, ALU_inp2 = 4;
    else
    { // 0 - inp1; 1 - inp2
        ALU_inp1 = forwarder_ex(IDEX, EXMO, old_MOWB, t1_forwards, t2_forwards, 0);
        ALU_inp2 = forwarder_ex(IDEX, EXMO, old_MOWB, t1_forwards, t2_forwards, 1);
    }
    ALU aluRes(IDEX.ALUsel, ALU_inp1, ALU_inp2);

    int JPC, BPC;
    // JPC and BPC computation
    if (IDEX.CW.jump != "00")
    {
        PC.IA = JPC;
        PC.valid = true;
        IFID.valid = false;
        IDEX.valid = false;
        IFID.stall = false;
        total_flushes++;
    }
    else if ((IDEX.CW.branch == "01" && aluRes.zeroFlag) || (IDEX.CW.branch == "10" && aluRes.LTflag))
    {
        PC.IA = BPC;
        PC.valid = true;
        IFID.valid = false;
        IDEX.valid = false;
        IFID.stall = false;
        total_flushes++;
    }
    // EXMO writing
    IDEX.stall = false;
    EXMO.valid = true;
}

void memory_op(std::vector<int> &DM, exmo &EXMO, mowb &MOWB, mowb &old_MOWB, int &DM_write, int &DM_read, int &t3_forwards)
{
    if (MOWB.valid)
        old_MOWB = MOWB;
    if (!EXMO.valid)
    {
        if (!MOWB.stall)
            MOWB.valid = false;
        return;
    }
    if (MOWB.stall)
    {
        EXMO.stall = true;
        return;
    }
    int mem_inp = forwarder_mo(EXMO, MOWB, t3_forwards);
    // Mem op and MOWB write
    EXMO.stall = false;
    MOWB.valid = true;
}

void writeback(int regFile[], mowb &MOWB, int &reg_write_count, int &instr_count)
{
    if (!MOWB.valid)
        return;
    if (MOWB.stall)
        return;
    if (MOWB.CW.regWrite)
    {
        if (MOWB.CW.mem2Reg)
            regFile[MOWB.rdl] = MOWB.LDres;
        else
            regFile[MOWB.rdl] = MOWB.ALUres;
    }
    MOWB.stall = false;
    regFile[0] = 0;
    instr_count++;
}

int main(int argc, char *argv[])
{

    return 0;
}