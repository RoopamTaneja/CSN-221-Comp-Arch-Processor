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
    if (!EXMO.CW.memRead) // producer must be a load
        return false;
    if (check == "100" || check == "101") // no hazard for j-type, u-type
        return false;
    if (EXMO.rdl == IDEX.rsl1)
        return true;
    if ((check == "000" || check == "011") && (EXMO.rdl == IDEX.rsl2)) // rsl2 dep for r-type, b-type
        return true;

    return false;
}

bool rs1_hazard(idex &IDEX, exmo &EXMO, mowb &MOWB, bool choice)
{
    bool hazard_flag = false;
    if (!choice)
    {
        if (EXMO.CW.immSel == "010" || EXMO.CW.immSel == "011") // no hazard if producer is b-type, s-type
            hazard_flag = false;
        if (EXMO.CW.memRead) // no hazard here if producer is load
            hazard_flag = false;
        if (IDEX.CW.immSel == "100" || IDEX.CW.immSel == "101") // no hazard if consumer is j-type, u-type
            hazard_flag = false;
        if (EXMO.rdl == IDEX.rsl1)
            hazard_flag = true;
    }
    else
    {
        if (MOWB.CW.immSel == "010" || MOWB.CW.immSel == "011") // no hazard if producer is b-type, s-type
            hazard_flag = false;
        if (IDEX.CW.immSel == "100" || IDEX.CW.immSel == "101") // no hazard if consumer is j-type, u-type
            hazard_flag = false;
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
        if (EXMO.CW.immSel == "010" || EXMO.CW.immSel == "011") // no hazard if producer is b-type, s-type
            hazard_flag = false;
        if (EXMO.CW.memRead) // no hazard here if producer is load
            hazard_flag = false;
        if (IDEX.CW.immSel == "001" || IDEX.CW.immSel == "100" || IDEX.CW.immSel == "101" || IDEX.CW.immSel == "010")
            hazard_flag = false; // no hazard if consumer is j-type, u-type, i-type, load, s-type
        if (EXMO.rdl == IDEX.rsl2)
            hazard_flag = true;
    }
    else
    {
        if (MOWB.CW.immSel == "010" || MOWB.CW.immSel == "011") // no hazard if producer is b-type, s-type
            hazard_flag = false;
        if (IDEX.CW.immSel == "001" || IDEX.CW.immSel == "100" || IDEX.CW.immSel == "101")
            hazard_flag = false; // no hazard if consumer is j-type, u-type, i-type, load
        if (MOWB.rdl == IDEX.rsl2)
            hazard_flag = true;
    }
    return hazard_flag;
}

bool sw_rs2_hazard(exmo &EXMO, mowb &MOWB)
{
    bool hazard_flag = false;
    if (MOWB.CW.immSel == "010" || MOWB.CW.immSel == "011") // no hazard if producer is b-type, s-type
        hazard_flag = false;
    if (EXMO.CW.immSel == "010")
        hazard_flag = false; // no hazard if consumer is not s-type
    if (MOWB.rdl == EXMO.rsl2)
        hazard_flag = true;

    return hazard_flag;
}

int forwarder_ex(idex &IDEX, exmo &EXMO, mowb &MOWB, bool choice, int &total_forwards)
{
    int operand;
    if (!choice)
    {
        if (rs1_hazard(IDEX, EXMO, MOWB, 0))
        {
            operand = EXMO.ALUres;
            total_forwards++;
        }
        else if (rs1_hazard(IDEX, EXMO, MOWB, 1))
        {
            if (MOWB.CW.memRead)
                operand = MOWB.LDres;
            else
                operand = MOWB.ALUres;
            total_forwards++;
        }
        else
            operand = IDEX.rs1;
    }
    else
    {
        if (rs2_hazard(IDEX, EXMO, MOWB, 0))
        {
            operand = EXMO.ALUres;
            total_forwards++;
        }
        else if (rs2_hazard(IDEX, EXMO, MOWB, 1))
        {
            if (MOWB.CW.memRead)
                operand = MOWB.LDres;
            else
                operand = MOWB.ALUres;
            total_forwards++;
        }
        else
            operand = IDEX.rs2;
    }
    return operand;
}

int forwarder_mo(exmo &EXMO, mowb &MOWB, int &total_forwards)
{
    if (sw_rs2_hazard(EXMO, MOWB))
    {
        total_forwards++;
        return MOWB.LDres;
    }
    return EXMO.rdl;
}

string instr_fetch(const std::vector<string> &IM, pc &PC, ifid &IFID, int &IM_access_count)
{
    if (!PC.valid)
    {
        if (!IFID.stall)
            IFID.valid = false;
        return "\tStage 1: -NIL-\t";
    }
    string ans = ("\tStage 1: ins " + std::to_string(PC.IA / 4) + "\t");
    if (IFID.stall)
        return ans;

    IM_access_count++;
    IFID.instr_reg = IM[PC.IA / 4];
    IFID.instr_PC = PC.IA;
    PC.IA += 4;
    IFID.valid = true;
    return ans;
}

string instr_decode(std::vector<int> &regFile, ifid &IFID, idex &IDEX, int &decode_count, int &reg_read_count)
{
    if (!IFID.valid)
    {
        if (!IDEX.stall)
            IDEX.valid = false;
        return "\tStage 2: -NIL-\t";
    }
    string ans = ("\tStage 2: ins " + std::to_string(IFID.instr_PC / 4) + "\t");
    if (IDEX.stall)
    {
        IFID.stall = true;
        return ans;
    }

    IDEX.instr_PC = IFID.instr_PC;
    decode_count++;
    string op5 = IFID.instr_reg.substr(25, 5);
    string f3 = IFID.instr_reg.substr(17, 3);
    char f7 = IFID.instr_reg[6];
    Controller CW(op5, f3);
    IDEX.CW = CW;
    IDEX.ALUsel = ALUcontrol(IDEX.CW.ALUop, f3, f7);
    IDEX.imm = immGen(IFID.instr_reg, CW.immSel);

    IDEX.rsl1 = stoi(IFID.instr_reg.substr(12, 5), NULL, 2);
    IDEX.rsl2 = stoi(IFID.instr_reg.substr(7, 5), NULL, 2);
    IDEX.rs1 = 0, IDEX.rs2 = 0;
    if (IDEX.CW.op1Sel) // auipc
        IDEX.rs1 = IDEX.instr_PC;
    else
    {
        reg_read_count++;
        IDEX.rs1 = regFile[IDEX.rsl1];
    }

    if (IDEX.CW.op2Sel)
        IDEX.rs2 = IDEX.imm;
    else
    {
        reg_read_count++;
        IDEX.rs2 = regFile[IDEX.rsl2];
    }

    if ((IDEX.CW.immSel == "010"))
    {
        reg_read_count++;
        IDEX.rdl = regFile[IDEX.rsl2]; // forwarding rs2 for sw
    }
    else
        IDEX.rdl = stoi(IFID.instr_reg.substr(20, 5), NULL, 2);

    IFID.stall = false;
    IDEX.valid = true;
    return ans;
}

string instr_execute(idex &IDEX, exmo &EXMO, ifid &IFID, mowb &MOWB, pc &PC, int &alu_count, int &total_stalls, int &total_flushes, int &total_forwards)
{
    if (!IDEX.valid)
    {
        if (!EXMO.stall)
            EXMO.valid = false;
        return "\tStage 3: -NIL-\t";
    }
    string ans = ("\tStage 3: ins " + std::to_string(IDEX.instr_PC / 4) + "\t");
    if (load_use_hazard(IDEX, EXMO))
    {
        total_stalls++;
        IDEX.stall = true;
        EXMO.valid = false;
        EXMO.stall = true;
        return ans;
    }
    if (EXMO.stall)         // stalling for only one cycle
        IDEX.stall = false; // so if already stalled => now you can remove stall

    alu_count++;
    int ALU_inp1, ALU_inp2;
    if (IDEX.CW.jump != "00")
        ALU_inp1 = IDEX.instr_PC, ALU_inp2 = 4;
    else
    {
        ALU_inp1 = forwarder_ex(IDEX, EXMO, MOWB, 0, total_forwards);
        ALU_inp2 = forwarder_ex(IDEX, EXMO, MOWB, 1, total_forwards);
    }
    ALU aluRes(IDEX.ALUsel, ALU_inp1, ALU_inp2);

    int JPC;
    if (IDEX.CW.jump == "01") // jal
        JPC = IDEX.instr_PC + IDEX.imm;
    else if (IDEX.CW.jump == "10") // jalr
        JPC = IDEX.rs1 + IDEX.imm;

    int BPC = IDEX.instr_PC + IDEX.imm;
    if (IDEX.CW.jump != "00")
    {
        PC.IA = JPC;
        PC.valid = true;
        IFID.valid = false;
        IDEX.valid = false;
        total_flushes++;
    }
    else if ((IDEX.CW.branch == "01" && aluRes.zeroFlag) || (IDEX.CW.branch == "10" && aluRes.LTflag))
    {
        PC.IA = BPC;
        PC.valid = true;
        IFID.valid = false;
        IDEX.valid = false;
        total_flushes++;
    }
    EXMO.instr_PC = IDEX.instr_PC;
    EXMO.ALUres = aluRes.ALUresult;
    EXMO.rsl2 = IDEX.rsl2;
    EXMO.rdl = IDEX.rdl;
    EXMO.CW = IDEX.CW;
    IDEX.stall = false;
    EXMO.valid = true;
    return ans;
}

string memory_op(std::vector<int> &DM, exmo &EXMO, mowb &MOWB, int &DM_write, int &DM_read, int &total_forwards)
{
    if (!EXMO.valid)
    {
        if (!MOWB.stall)
            MOWB.valid = false;
        return "\tStage 4: -NIL-\t";
    }
    string ans = ("\tStage 4: ins " + std::to_string(EXMO.instr_PC / 4) + "\t");
    if (MOWB.stall)
    {
        EXMO.stall = true;
        return ans;
    }

    int mem_inp = forwarder_mo(EXMO, MOWB, total_forwards);
    if (EXMO.CW.memWrite && EXMO.CW.regRead)
    {
        DM_write++;
        DM[EXMO.ALUres / 4] = mem_inp;
    }
    if (EXMO.CW.memRead)
    {
        DM_read++;
        MOWB.LDres = DM[EXMO.ALUres / 4];
    }
    MOWB.instr_PC = EXMO.instr_PC;
    MOWB.CW = EXMO.CW;
    MOWB.ALUres = EXMO.ALUres;
    MOWB.rdl = EXMO.rdl;
    EXMO.stall = false;
    MOWB.valid = true;
    return ans;
}

string writeback(std::vector<int> &regFile, mowb &MOWB, int &reg_write_count, int &instr_count)
{
    if (!MOWB.valid)
        return "\tStage 5: -NIL-\n";
    string ans = "\tStage 5: ins " + std::to_string(MOWB.instr_PC / 4) + "\n";
    if (MOWB.stall)
        return ans;
    if (MOWB.CW.regWrite)
    {
        reg_write_count++;
        if (MOWB.CW.mem2Reg)
            regFile[MOWB.rdl] = MOWB.LDres;
        else
            regFile[MOWB.rdl] = MOWB.ALUres;
    }
    MOWB.stall = false;
    regFile[0] = 0;
    instr_count++;
    return ans;
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        cout << "Incorrect Format\n";
        return 0;
    }
    string instrFile = argv[1];
    string dataFile = argv[2];
    string cycleFile = argv[3];
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

    // For outputting cycle-wise stage description
    std::ofstream outCycle(cycleFile, std::ios::trunc);
    if (outCycle.bad())
    {
        cout << "Error in opening : " << cycleFile << "\n";
        return 0;
    }

    // Pipeline
    int instr_count = 0, total_stalls = 0, total_flushes = 0, total_forwards = 0;
    int IM_access_count = 0, decode_count = 0, reg_read_count = 0, alu_count = 0;
    int DM_write = 0, DM_read = 0, reg_write_count = 0;

    std::vector<int> regFile(32, 0);
    pc PC;
    ifid IFID;
    idex IDEX;
    exmo EXMO;
    mowb MOWB;
    PC.valid = true;
    PC.IA = 0;
    int cycle_no = 1;
    std::vector<string> stage_desc;
    while (PC.valid || IFID.valid || IDEX.valid || EXMO.valid || MOWB.valid)
    {
        outCycle << "\nCycle " << cycle_no << ":\n";
        if (PC.IA >= numInstr * 4)
            PC.valid = false;

        stage_desc.emplace_back(writeback(regFile, MOWB, reg_write_count, instr_count));
        stage_desc.emplace_back(memory_op(DM, EXMO, MOWB, DM_write, DM_read, total_forwards));
        stage_desc.emplace_back(instr_execute(IDEX, EXMO, IFID, MOWB, PC, alu_count, total_stalls, total_flushes, total_forwards));
        stage_desc.emplace_back(instr_decode(regFile, IFID, IDEX, decode_count, reg_read_count));
        stage_desc.emplace_back(instr_fetch(IM, PC, IFID, IM_access_count));

        for (int i = 4; i >= 0; i--)
            outCycle << stage_desc[i];
        stage_desc.clear();
        cycle_no++;
    }
    outCycle.close();
    cout << "\nExecution statistics of 5-stage pipeline with operand forwarding: \n";
    cout << "\nNo of accesses of Instruction Memory accesses: " << IM_access_count << "\n";
    cout << "No of accesses of Decode Unit: " << decode_count << "\n";
    cout << "No of reads from Register File: " << reg_read_count << "\n";
    cout << "No of accesses of ALU Unit: " << alu_count << "\n";
    cout << "No of writes into Data Memory: " << DM_write << "\n";
    cout << "No of reads from Data Memory: " << DM_read << "\n";
    cout << "No of writes into Register File: " << reg_write_count << "\n";
    cout << "-----------------------------------------------------------------\n";
    cout << "No of machine-level instructions (I) = " << instr_count << "\n";
    cout << "Total no of forwards (data hazards) = " << total_forwards << "\n";
    cout << "Total no of stalls (load use hazards) = " << total_stalls << "\n";
    cout << "Total no of flushes (branch penalties) = " << total_flushes << "\n";
    cout << "Total no of cycles in simulator = " << cycle_no - 1 << "\n";
    cout << "Actual total no of cycles (if 2 stalls per branch penalty) = " << cycle_no - 1 + total_flushes << "\n";
    cout << "Hence cycles wasted (S) = " << total_stalls + total_flushes * 2 << "\n";
    cout << "\nCPI = " << ((float)(cycle_no - 1 + total_flushes)) / instr_count << "\n";

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