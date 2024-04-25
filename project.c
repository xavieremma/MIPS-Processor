/* I  have  not  used  C  language  code  obtained  from  other  students,
the Internet,  and  any  other  unauthorized  sources,  either  modified  or
unmodified. If any code in my program was obtained from an authorized
source, such as textbook or course notes, that has been clearly noted
as a citation in the comments of the program.
*/
// Xavier Emma
// xa296047

#include "spimcore.h"

/* ALU */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    // Perform ALU operation based on ALUControl
    if (ALUControl == 0)
        *ALUresult = A + B;
    else if (ALUControl == 1)
        *ALUresult = A - B;
    else if (ALUControl == 2)
        *ALUresult = (int) A < (int) B ? 1 : 0;
    else if (ALUControl == 3)
        *ALUresult = A < B ? 1 : 0;
    else if (ALUControl == 4)
        *ALUresult = A & B;
    else if (ALUControl == 5)
        *ALUresult = A | B;
    else if (ALUControl == 6)
        *ALUresult = B << 16;
    else if (ALUControl == 7)
        *ALUresult = ~A;
    *Zero = *ALUresult == 0 ? 1 : 0;
    return;
}

/* Instruction Fetch */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    // Check if PC is a multiple of 4 (word-aligned)
    if (PC % 4 != 0)
        return 1;
    // check PC multiple of 4
   
    *instruction = Mem[PC >> 2];
    // input fetched from mem at PC
    return 0;
}

/* Instruction Partition */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    unsigned opcode_mask = 0xFC000000;
    unsigned r1_mask = 0x03E00000;
    unsigned r2_mask = 0x001F0000;
    unsigned r3_mask = 0x0000F800;
    unsigned funct_mask = 0x0000003F;
    unsigned offset_mask = 0x0000FFFF;
    unsigned jsec_mask = 0x03FFFFFF;
    // instruction bitmasks
    
    *op = instruction & opcode_mask;
    *r1 = (instruction & r1_mask) >> 21;
    *r2 = (instruction & r2_mask) >> 16;
    *r3 = (instruction & r3_mask) >> 11;
    *funct = instruction & funct_mask;
    *offset = instruction & offset_mask;
    *jsec = instruction & jsec_mask;
    // grab intrsuction fields
}

/* Instruction Decode */
int instruction_decode(unsigned op, struct_controls *controls)
{
    controls->RegDst = 0;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->MemRead = 0;
    controls->MemtoReg = 0;
    controls->ALUOp = 0;
    controls->MemWrite = 0;
    controls->ALUSrc = 0;
    controls->RegWrite = 0;
    // control signals are reset

    switch(op) {
    case 0x00000000: // R-type
        controls->RegDst = 1;
        controls->ALUOp = 7;
        controls->RegWrite = 1;
        break;
    case 0x20000000: // ADDI
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        break;
    case 0x8c000000: // LW
        controls->ALUSrc = 1;
        controls->MemRead = 1;
        controls->MemtoReg = 1;
        controls->RegWrite = 1;
        break;
    case 0xac000000: // SW
        controls->ALUSrc = 1;
        controls->MemWrite = 1;
        break;
    case 0x3c000000: // LUI
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        controls->ALUOp = 6;
        break;
    case 0x28000000: // SLTI
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        controls->ALUOp = 2;
        break;
    case 0x2c000000: // SLTIU
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        controls->ALUOp = 3;
        break;
    case 0x10000000: // BEQ
        controls->ALUOp = 1;
        controls->Branch = 1;
        break;
    case 0x08000000: // JUMP
        controls->Jump = 1;
        break;
    default:
        
        return 1;
            // opcode is invalid
    }

    return 0;
    // control signals based on opcode
}

/* Read Register */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    *data1 = Reg[r1];
    *data2 = Reg[r2];
    // read data from registers 1 and 2
}

/* Sign Extend */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    *extended_value = offset & 0x0000FFFF;
    //Assigns sign_extended value offset to extended_value.
    
    if (offset & 0x00008000)
        *extended_value |= 0xFFFF0000;
    
    return;
}
/* DECODE R*/
int decode_r(unsigned data1, unsigned data2, unsigned funct, unsigned *ALUresult, char *Zero) {
    switch (funct) {
        case 0x20:
            // ADD
            ALU(data1, data2, 0, ALUresult, Zero);
            break;
        case 0x22:
            // SUB
            ALU(data1, data2, 1, ALUresult, Zero);
            break;
        case 0x24:
            // AND
            ALU(data1, data2, 4, ALUresult, Zero);
            break;
        case 0x25:
            // OR
            ALU(data1, data2, 5, ALUresult, Zero);
            break;
        case 0x2A:
            // SLT
            ALU(data1, data2, 2, ALUresult, Zero);
            break;
        case 0x2B:
            // SLTU
            ALU(data1, data2, 3, ALUresult, Zero);
            break;
        default:
            return 1;
    }
    // INSTRUCTION FOR DECODING R
    return 0;
}

/* ALU Operations */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    // Set ALUOp based on funct field for R-type instructions
    switch (ALUOp) {
        case 0:
            // ADD
            ALU(data1, ALUSrc == 1 ? extended_value : data2, ALUOp, ALUresult, Zero);
            break;
        case 1:
            // Subtract
            ALU(data1, ALUSrc == 1 ? extended_value : data2, ALUOp, ALUresult, Zero);
            break;
        case 2:
            // Set less than
            ALU(data1, ALUSrc == 1 ? extended_value : data2, ALUOp, ALUresult, Zero);
            break;
        case 3:
            // SLTIU
            ALU(data1, ALUSrc == 1 ? extended_value : data2, ALUOp, ALUresult, Zero);
            break;
        case 4:
            // AND
            ALU(data1, ALUSrc == 1 ? extended_value : data2, ALUOp, ALUresult, Zero);
            break;
        case 5:
            // OR
            ALU(data1, ALUSrc == 1 ? extended_value : data2, ALUOp, ALUresult, Zero);
            break;
        case 6:
            // SLL
            ALU(data1, ALUSrc == 1 ? extended_value : data2, ALUOp, ALUresult, Zero);
            break;
        case 7:
            // R-type instruction
            return decode_r(data1, data2, funct, ALUresult, Zero);
            break;
        default:
            return 1;
    }
    return 0;
}

/* Read / Write Memory */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    if (MemRead == 0x1 || MemWrite == 0x1) {
        if ((ALUresult % 4) != 0)
            // addres is word alligned check
            return 1; // Halt
    
        if (ALUresult < 0x0000 || ALUresult > 0xFFFF)
            return 1;
        // address is in range check
    }
    // address is word-aligned + range check

    if (MemRead == 0x1 && MemWrite == 0x0) {
        *memdata = Mem[ALUresult >> 2];
        // loads word into mem
    }

    if (MemRead == 0x0 && MemWrite == 0x1) {
        Mem[ALUresult >> 2] = data2;
        // stores word into mem
    }

    return 0;
}

/* Write Register */
void write_register(unsigned r2, unsigned r3, unsigned memdata, unsigned ALUresult, char RegWrite, char RegDst, char MemtoReg, unsigned *Reg)
{
    if (RegDst == 0x1 && MemtoReg == 0x0 && RegWrite == 0x1) {
        if (r3 == 0) return;
        // r-type instructions write to register 3
        Reg[r3] = ALUresult;
        return;
    }
    if (RegDst == 0x0 && MemtoReg == 0x1 && RegWrite == 0x1) {
        if (r2 == 0) return;
        // load word, write to register 2
        Reg[r2] = memdata;
        return;
    }
    if (RegDst == 0x0 && MemtoReg == 0x0 && RegWrite == 0x1) {
        if (r2 == 0) return;
        // ADDI, LUI, SLTI, SLTIU, write to register 2
        Reg[r2] = ALUresult;
    }
}

/* PC Update */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{

   
    if (Branch == 0 && Jump == 0) {
        *PC = *PC + 4;
        // increments PC by 4
    }
    if (Branch == 1 && Jump == 0 && Zero == 1) {
        *PC = (extended_value << 2) + (*PC + 4);
        // branch address in PC + extended_value
    }
    if (Branch == 0 && Jump == 1) {
        // jump to address in jsec
        *PC = ((*PC + 4) & 0xf0000000) | (jsec << 2);
        // jump
    }
    // branch equal
}
