// // 5-stage pipelined RISC-V processor simulator with stalls

// #include <iostream>
// #include <iomanip>
// #include <chrono>
// #include <thread>
// #include <vector>
// #include <fstream>
// #include <sstream>
// using std::cout, std::cin, std::string;

// int dec_sign_imm(string s)
// {
//     int dec_val = 0, base = 1;
//     bool neg = (s[0] == '1');
//     if (neg)
//     {
//         int k = s.find_last_of('1');
//         for (int i = k - 1; i >= 0; i--)
//             s[i] == '0' ? s[i] = '1' : s[i] = '0';
//     }
//     int len = s.length();
//     for (int i = len - 1; i >= 0; i--)
//     {
//         if (s[i] == '1')
//             dec_val += base;
//         base = base * 2;
//     }
//     if (neg)
//         return -1 * dec_val;
//     return dec_val;
// }

// class Controller
// {
// public:
//     int instr_id;
//     bool op1Sel, op2Sel;
//     string immSel, ALUop;
//     bool regRead, regWrite;
//     bool memRead, memWrite, mem2Reg;
//     string branch, jump;

//     void setImmSel_n_ALUop(string op5)
//     {
//         if (op5 == "01100")
//             immSel = "000", ALUop = "00";
//         else if (op5 == "00100")
//             immSel = "001", ALUop = "01";
//         else if (op5 == "00000" || op5 == "11001")
//             immSel = "001", ALUop = "10";
//         else if (op5 == "01000")
//             immSel = "010", ALUop = "10";
//         else if (op5 == "11000")
//             immSel = "011", ALUop = "11";
//         else if (op5 == "11011")
//             immSel = "100", ALUop = "10";
//         else
//             immSel = "101", ALUop = "10";
//     }

//     Controller(int pc, string op5, string f3)
//     {
//         instr_id = pc / 4;

//         if (op5 == "00101")
//             op1Sel = 1;
//         else
//             op1Sel = 0;

//         if (op5 == "01100" || op5 == "11000" || op5 == "11011" || op5 == "11001")
//             op2Sel = 0;
//         else
//             op2Sel = 1;

//         setImmSel_n_ALUop(op5);

//         if (op5 == "11011" || op5 == "01101" || op5 == "00101")
//             regRead = 0;
//         else
//             regRead = 1;

//         if (op5 == "01000" || op5 == "11000")
//             regWrite = 0;
//         else
//             regWrite = 1;

//         if (op5 == "00000")
//             memRead = 1, mem2Reg = 1;
//         else
//             memRead = 0, mem2Reg = 0;

//         if (op5 == "01000")
//             memWrite = 1;
//         else
//             memWrite = 0;

//         if (op5 == "11000")
//         {
//             if (f3 == "000")
//                 branch = "01";
//             else if (f3 == "100")
//                 branch = "10";
//         }
//         else
//             branch = "00";

//         if (op5 == "11011")
//             jump = "01";
//         else if (op5 == "11001")
//             jump = "10";
//         else
//             jump = "00";
//     }

//     void checkCtrlWord()
//     {
//         cout << op1Sel << op2Sel << immSel << ALUop << regRead << regWrite << memRead
//              << memWrite << mem2Reg << branch << jump << "\n";
//     }
// };

// int immGen(const string instr, string immSel)
// {
//     int imm;
//     if (immSel == "000") // no imm needed
//         imm = INT32_MIN;
//     else if (immSel == "001") // i-type imm
//         imm = dec_sign_imm(instr.substr(0, 12));
//     else if (immSel == "010") // s-type imm
//         imm = dec_sign_imm(instr.substr(0, 7) + instr.substr(20, 5));
//     else if (immSel == "011") // b-type imm
//     {
//         string offset = "";
//         offset += instr[0] + instr[24];
//         offset += instr.substr(1, 6) + instr.substr(20, 4);
//         imm = dec_sign_imm(offset);
//         imm <<= 1;
//     }
//     else if (immSel == "100") // j type imm
//     {
//         string offset = "";
//         offset += instr[0];
//         offset += instr.substr(12, 8);
//         offset += instr[11];
//         offset += instr.substr(1, 10);
//         imm = dec_sign_imm(offset);
//         imm <<= 1;
//     }
//     else if (immSel == "101") // u type imm
//     {
//         imm = dec_sign_imm(instr.substr(0, 20));
//         imm <<= 12;
//     }
//     return imm;
// }

// string ALUcontrol(string ALUop, string f3, char f7)
// {
//     if (ALUop == "10")
//         return "0000";
//     if (ALUop == "11")
//         return "0001";
//     if (ALUop == "00") // r-type
//     {
//         if (f3 == "000" && f7 == '0')
//             return "0000";
//         if (f3 == "010" && f7 == '0')
//             return "0001";
//         if (f3 == "100" && f7 == '0')
//             return "0101";
//         if (f3 == "110" && f7 == '0')
//             return "0110";
//         if (f3 == "111" && f7 == '0')
//             return "0111";
//         if (f3 == "001" && f7 == '0')
//             return "1000";
//         if (f3 == "101" && f7 == '0')
//             return "1001";
//         if (f3 == "110" && f7 == '1')
//             return "0100";
//         if (f3 == "000" && f7 == '1')
//             return "0010";
//         if (f3 == "100" && f7 == '1')
//             return "0011";
//     }
//     // i-type
//     if (f3 == "000")
//         return "0000";
//     if (f3 == "100")
//         return "0101";
//     if (f3 == "110")
//         return "0110";
//     if (f3 == "111")
//         return "0111";
//     if (f3 == "001")
//         return "1000";
//     return "1001";
// }

// class ALU
// {
// public:
//     int ALUresult;
//     bool zeroFlag;
//     bool LTflag;
//     ALU(){};
//     ALU(string ALUsel, int rs1, int rs2)
//     {
//         if (ALUsel == "0000")
//             ALUresult = rs1 + rs2;
//         else if (ALUsel == "0001")
//             ALUresult = rs1 - rs2;
//         else if (ALUsel == "0010")
//             ALUresult = rs1 * rs2;
//         else if (ALUsel == "0011")
//             ALUresult = rs1 / rs2;
//         else if (ALUsel == "0100")
//             ALUresult = rs1 % rs2;
//         else if (ALUsel == "0101")
//             ALUresult = rs1 ^ rs2;
//         else if (ALUsel == "0110")
//             ALUresult = rs1 | rs2;
//         else if (ALUsel == "0111")
//             ALUresult = rs1 & rs2;
//         else if (ALUsel == "1000")
//             ALUresult = rs1 << rs2;
//         else if (ALUsel == "1001")
//             ALUresult = rs1 >> rs2;

//         zeroFlag = (ALUresult == 0);
//         LTflag = (ALUresult < 0);
//     }
// };

// class gpr
// {
// public:
//     int instr_id = -1;
//     int value = 0;
//     gpr(int id = -1, int val = 0) : instr_id(id), value(val) {}
// };

// class pipelineRegister
// {
// public:
//     bool valid = false, stall = false;
// };

// class pc : public pipelineRegister
// {
// public:
//     int IA;
// };

// class ifid : public pipelineRegister
// {
// public:
//     int instr_PC;
//     string instr_reg;
// };

// class idex : public pipelineRegister
// {
// public:
//     int instr_PC;
//     int imm;
//     Controller CW;
//     int rs1, rs2, rdl;
//     string ALUsel;
//     idex() : CW(-1, "00000", "xxx") {}
// };

// class exmo : public pipelineRegister
// {
// public:
//     Controller CW;
//     int ALUres;
//     int rs2, rdl;
//     exmo() : CW(-1, "00000", "xxx") {}
// };

// class mowb : public pipelineRegister
// {
// public:
//     Controller CW;
//     int ALUres, LDres;
//     int rdl;
//     mowb() : CW(-1, "00000", "xxx") {}
// };

// string instr_fetch(const std::vector<string> &IM, pc &PC, ifid &IFID)
// {
//     if (!PC.valid)
//     {
//         if (!IFID.stall)
//             IFID.valid = false;
//         return "\tStage 1: -NIL-\t";
//     }
//     string ans = ("\tStage 1: ins " + std::to_string(PC.IA / 4) + "\t");
//     if (IFID.stall)
//         return ans;

//     std::this_thread::sleep_for(std::chrono::microseconds(200));
//     IFID.instr_reg = IM[PC.IA / 4];
//     IFID.instr_PC = PC.IA;
//     PC.IA += 4;
//     IFID.valid = true;
//     return ans;
// }

// string instr_decode(std::vector<gpr> &regFile, ifid &IFID, idex &IDEX)
// {
//     if (!IFID.valid)
//     {
//         if (!IDEX.stall)
//             IDEX.valid = false;
//         return "\tStage 2: -NIL-\t";
//     }
//     string ans = ("\tStage 2: ins " + std::to_string(IFID.instr_PC / 4) + "\t");
//     if (IDEX.stall)
//     {
//         IFID.stall = true;
//         return ans;
//     }

//     IDEX.instr_PC = IFID.instr_PC;
//     std::this_thread::sleep_for(std::chrono::microseconds(250));
//     string op5 = IFID.instr_reg.substr(25, 5);
//     string f3 = IFID.instr_reg.substr(17, 3);
//     char f7 = IFID.instr_reg[6];
//     Controller CW(IFID.instr_PC, op5, f3);
//     IDEX.CW = CW;
//     IDEX.ALUsel = ALUcontrol(IDEX.CW.ALUop, f3, f7);
//     IDEX.imm = immGen(IFID.instr_reg, CW.immSel);

//     int rsl1 = stoi(IFID.instr_reg.substr(12, 5), NULL, 2);
//     int rsl2 = stoi(IFID.instr_reg.substr(7, 5), NULL, 2);
//     IDEX.rs1 = 0, IDEX.rs2 = 0;
//     if (IDEX.CW.op1Sel) // auipc
//         IDEX.rs1 = IDEX.instr_PC;
//     else
//     {
//         std::this_thread::sleep_for(std::chrono::microseconds(25));
//         if (regFile[rsl1].instr_id == -1)
//             IDEX.rs1 = regFile[rsl1].value;
//         else
//         { // Stalled
//             IDEX.rs1 = rsl1;
//             IFID.stall = true;
//             IDEX.valid = false;
//             IDEX.stall = true;
//             return ans;
//         }
//     }

//     if (IDEX.CW.op2Sel)
//         IDEX.rs2 = IDEX.imm;
//     else
//     {
//         std::this_thread::sleep_for(std::chrono::microseconds(25));
//         if (regFile[rsl2].instr_id == -1)
//             IDEX.rs2 = regFile[rsl2].value;
//         else
//         { // Stalled
//             IDEX.rs1 = rsl2;
//             IFID.stall = true;
//             IDEX.valid = false;
//             IDEX.stall = true;
//             return ans;
//         }
//     }
//     if ((IDEX.CW.immSel == "010"))
//     {
//         std::this_thread::sleep_for(std::chrono::microseconds(25));
//         if (regFile[rsl2].instr_id == -1)
//             IDEX.rdl = regFile[rsl2].value; // forwarding rs2 for sw
//         else
//         { // Stalled
//             IDEX.rs1 = rsl2;
//             IFID.stall = true;
//             IDEX.valid = false;
//             IDEX.stall = true;
//             return ans;
//         }
//     }
//     else if (IDEX.CW.immSel != "011") // lock when it's not B-type or S-type
//     {
//         IDEX.rdl = stoi(IFID.instr_reg.substr(20, 5), NULL, 2);
//         regFile[IDEX.rdl].instr_id = IDEX.CW.instr_id;
//     }

//     IFID.stall = false;
//     IDEX.valid = true;
//     return ans;
// }

// string instr_execute(idex &IDEX, exmo &EXMO, ifid &IFID, pc &PC)
// {
//     if (!IDEX.valid)
//     {
//         if (!EXMO.stall)
//             EXMO.valid = false;
//         return "\tStage 3: -NIL-\t";
//     }
//     string ans = ("\tStage 3: ins " + std::to_string(IDEX.CW.instr_id) + "\t");
//     if (EXMO.stall)
//     {
//         IDEX.stall = true;
//         return ans;
//     }

//     std::this_thread::sleep_for(std::chrono::microseconds(100));
//     ALU aluRes;
//     if (IDEX.CW.jump != "00")
//     {
//         ALU res(IDEX.ALUsel, IDEX.instr_PC, 4);
//         aluRes = res;
//     }
//     else
//     {
//         ALU res(IDEX.ALUsel, IDEX.rs1, IDEX.rs2);
//         aluRes = res;
//     }

//     int JPC;
//     if (IDEX.CW.jump == "01") // jal
//         JPC = IDEX.instr_PC + IDEX.imm;
//     else if (IDEX.CW.jump == "10") // jalr
//         JPC = IDEX.rs1 + IDEX.imm;

//     int BPC = IDEX.instr_PC + IDEX.imm;
//     if (IDEX.CW.jump != "00")
//     {
//         PC.IA = JPC;
//         PC.valid = true;
//         IFID.valid = false;
//         IDEX.valid = false;
//     }
//     else if ((IDEX.CW.branch == "01" && aluRes.zeroFlag) || (IDEX.CW.branch == "10" && aluRes.LTflag))
//     {
//         PC.IA = BPC;
//         PC.valid = true;
//         IFID.valid = false;
//         IDEX.valid = false;
//     }

//     EXMO.ALUres = aluRes.ALUresult;
//     EXMO.rs2 = IDEX.rs2;
//     EXMO.rdl = IDEX.rdl;
//     EXMO.CW = IDEX.CW;
//     IDEX.stall = false;
//     EXMO.valid = true;
//     return ans;
// }

// string memory_op(std::vector<int> &DM, exmo &EXMO, mowb &MOWB)
// {
//     if (!EXMO.valid)
//     {
//         if (!MOWB.stall)
//             MOWB.valid = false;
//         return "\tStage 4: -NIL-\t";
//     }
//     string ans = ("\tStage 4: ins " + std::to_string(EXMO.CW.instr_id) + "\t");
//     if (MOWB.stall)
//     {
//         EXMO.stall = true;
//         return ans;
//     }
//     if (EXMO.CW.memWrite && EXMO.CW.regRead)
//     {
//         std::this_thread::sleep_for(std::chrono::microseconds(200));
//         DM[EXMO.ALUres / 4] = EXMO.rdl;
//     }
//     if (EXMO.CW.memRead)
//     {
//         std::this_thread::sleep_for(std::chrono::microseconds(200));
//         MOWB.LDres = DM[EXMO.ALUres / 4];
//     }
//     MOWB.CW = EXMO.CW;
//     MOWB.ALUres = EXMO.ALUres;
//     MOWB.rdl = EXMO.rdl;
//     EXMO.stall = false;
//     MOWB.valid = true;
//     return ans;
// }

// string writeback(std::vector<gpr> &regFile, mowb &MOWB, idex &IDEX)
// {
//     if (!MOWB.valid)
//         return "\tStage 5: -NIL-\n";
//     string ans = "\tStage 5: ins " + std::to_string(MOWB.CW.instr_id) + "\n";
//     if (MOWB.stall)
//         return ans;
//     if (MOWB.CW.regWrite && regFile[MOWB.rdl].instr_id == MOWB.CW.instr_id)
//     {
//         std::this_thread::sleep_for(std::chrono::microseconds(25));
//         if (MOWB.CW.mem2Reg)
//             regFile[MOWB.rdl].value = MOWB.LDres;
//         else
//             regFile[MOWB.rdl].value = MOWB.ALUres;

//         regFile[MOWB.rdl].instr_id = -1;
//         if (MOWB.rdl == IDEX.rs1)
//             IDEX.stall = false; // Decode and WB can happen in same cycle (Structural soln)
//     }
//     MOWB.stall = false;
//     regFile[0].value = 0;
//     return ans;
// }

// int main(int argc, char *argv[])
// {
//     if (argc != 4)
//     {
//         cout << "Incorrect Format\n";
//         return 0;
//     }
//     string instrFile = argv[1];
//     string dataFile = argv[2];
//     string cycleFile = argv[3];
//     string instrLine, dataLine;
//     // Loading the data into program DM
//     std::vector<int> DM;
//     std::ifstream inData(dataFile);
//     if (inData.bad())
//     {
//         cout << "Error in opening : " << dataFile << "\n";
//         return 0;
//     }
//     while (getline(inData, dataLine))
//     {
//         std::vector<string> tokens;
//         std::stringstream ss(dataLine);
//         string token;
//         while (getline(ss, token, ' '))
//             tokens.push_back(token);

//         DM.push_back(stoi(tokens[1]));
//     }
//     inData.close();
//     // Loading the instr into program IM
//     std::vector<string> IM;
//     std::ifstream inInstr(instrFile);
//     if (inInstr.bad())
//     {
//         cout << "Error in opening : " << instrFile << "\n";
//         return 0;
//     }
//     while (getline(inInstr, instrLine))
//         IM.push_back(instrLine);

//     inInstr.close();
//     int numInstr = IM.size();

//     // For outputting cycle-wise stage description
//     std::ofstream outCycle(cycleFile, std::ios::trunc);
//     if (outCycle.bad())
//     {
//         cout << "Error in opening : " << cycleFile << "\n";
//         return 0;
//     }

//     // Pipeline
//     auto start = std::chrono::high_resolution_clock::now();
//     auto total_duration = std::chrono::duration_cast<std::chrono::microseconds>(start - start);

//     std::vector<gpr> regFile(32, gpr());
//     pc PC;
//     ifid IFID;
//     idex IDEX;
//     exmo EXMO;
//     mowb MOWB;
//     PC.valid = true;
//     PC.IA = 0;
//     int cycle_no = 1;
//     std::vector<string> stage_desc;
//     while (PC.valid || IFID.valid || IDEX.valid || EXMO.valid || MOWB.valid)
//     {
//         auto cycle_start = std::chrono::high_resolution_clock::now();
//         outCycle << "\nCycle " << cycle_no << ": ";
//         if (PC.IA >= numInstr * 4)
//             PC.valid = false;
        
//         stage_desc.emplace_back(writeback(regFile, MOWB, IDEX));
//         stage_desc.emplace_back(memory_op(DM, EXMO, MOWB));
//         stage_desc.emplace_back(instr_execute(IDEX, EXMO, IFID, PC));
//         stage_desc.emplace_back(instr_decode(regFile, IFID, IDEX));
//         stage_desc.emplace_back(instr_fetch(IM, PC, IFID));

//         auto cycle_end = std::chrono::high_resolution_clock::now();
//         auto cycle_duration = std::chrono::duration_cast<std::chrono::microseconds>(cycle_end - cycle_start);
//         total_duration += cycle_duration;
//         outCycle << "Time = " << cycle_duration.count() << " microseconds\n";
//         for (int i = 4; i >= 0; i--)
//             outCycle << stage_desc[i];
//         stage_desc.clear();
//         cycle_no++;
//     }
//     outCycle.close();
//     cout << "Execution time of 5-stage pipeline with stalls: " << total_duration.count() << " microseconds\n";

//     // Printing back the data from DM
//     std::ofstream outData(dataFile, std::ios::trunc);
//     if (outData.bad())
//     {
//         cout << "Error in opening : " << dataFile << "\n";
//         return 0;
//     }
//     int addr = 0;
//     for (auto &d : DM)
//     {
//         outData << "0x" << std::setw(4) << std::setfill('0') << std::uppercase << std::hex << addr << ": " << std::dec << d << "\n";
//         addr += 4;
//     }
//     outData.close();
//     return 0;
// }