#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;

// enums
enum Operation {
    conditional_jump,
    immediate_to_register,
    immediate_to_register_mem,
    memory_to_acc_or_vv,
    register_mem_to_from_register,
    register_mem_to_from_seg_register,
    unknown
};

enum Mnemonic
{
    add,
    cmp,
    je,
    jl,
    jle,
    jb,
    jbe,
    jp,
    jo,
    js,
    jne,
    jnl,
    jg,
    jnb,
    ja,
    jnp,
    jno,
    jns,
    loop,
    loopz,
    loopnz,
    jcxz,
    mov,
    sub
};

enum WFlag
{
    Byte,
    Not_specified,
    Word
};

enum Lo_Hi_Byte {
    low_byte,
    high_byte,
    neither
};

enum MOD
{
    memory_mode,
    memory_mode_8_bit,
    memory_mode_16_bit,
    register_mode
};

enum class DispFlag
{
    Has_Displacement,
    No_Displacement
};

enum class RM
{
    al,
    cl,
    dl,
    bl,
    ah,
    ch,
    dh,
    bh,
    ax,
    cx,
    dx,
    bx,
    sp,
    bp,
    si,
    di,
    bx_plus_si,
    bx_plus_di,
    bp_plus_si,
    bp_plus_di,
    direct_address,
    bx_plus_si_plus8,
    bx_plus_di_plus8,
    bp_plus_si_plus8,
    bp_plus_di_plus8,
    si_plus8,
    di_plus8,
    bp_plus8,
    bx_plus8,
    bx_plus_si_plus16,
    bx_plus_di_plus16,
    bp_plus_si_plus16,
    bp_plus_di_plus16,
    si_plus16,
    di_plus16,
    bp_plus16,
    bx_plus16,
    not_set
};

enum class SR
{
    cs,
    ds,
    es,
    ss
};

enum Direction {
    accumulator_is_source,
    accumulator_is_destination,
    register_is_source,
    register_is_destination,
    segment_register_is_source,
    segment_register_is_destination
};

// Struct definitions
struct Immediate_to_register
{
    RM reg;
    int data;
    RM dest;
    int source;
};

struct Immediate_to_register_mem
{
    RM rm;
    MOD mod;
    int data;
    int disp;
    int s;
    int source;
    RM dest;
};

struct Memory_to_acc_or_vv
{
    int address;
    Direction d;
    RM operandOne;
    int operandTwo;
};

struct Register_mem_to_from_register
{
    RM rm;
    RM reg;
    MOD mod;
    Direction d;
    int disp;
    RM source;
    RM dest;
};

struct Register_mem_to_from_seg_register
{
    RM rm;
    MOD mod;
    Direction d;
    int disp;
    SR sr;
    RM operandOne;
    SR operandTwo;
};

struct Conditional_jump
{
    int data;
};

struct instruction
{
    Operation op_tag;
    WFlag w;
    int op_code;
    int byteTwo;
    Mnemonic mnemonic;

    union
    {
        Conditional_jump cond_jmp;
        Immediate_to_register imm_to_reg;
        Immediate_to_register_mem imm_to_reg_mem;
        Memory_to_acc_or_vv mem_to_acc;
        Register_mem_to_from_register reg_mem_to_from_reg;
        Register_mem_to_from_seg_register reg_mem_to_from_seg_reg;
    };

    instruction(int op_code, Operation op_tag, Mnemonic mnemonic)
    {
        this->op_code = op_code;
        this->op_tag = op_tag;
        this->mnemonic = mnemonic;
    }

    instruction(Operation op_tag)
    {
        this->op_tag = op_tag;
    }
};

struct CPU
{
    i16 regSlots[13] = {};
};

struct Memory
{
    i8 memSlots[65536] = {};
};

struct Flags
{
    bool flags[16] = {};
};

// Register & flags list
string regList[13] = {"ax", "bx", "cx", "dx", "sp", "bp", "si", "di", "es", "cs", "ss", "ds", "ip"};
string flagsList[16] = {"", "", "", "", "O", "D", "I", "T", "S", "Z", "", "A", "", "P", "", "C"};
int flagsListMask[9] = {4, 5, 6, 7, 8, 9, 11, 13, 15};

// Masks
const int singBitConv = 0b00000001;
const int twoBitConv = 0b00000011;
const int threeBitconv = 0b00000111;
const int fourBitConv = 0b00001111;
const int sixBitConv = 0b00111111;
const int sevBitConv = 0b01111111;
const int highBitsMask = 0b1111111100000000;
const int lowBitsMask = 0b0000000011111111;
const int sixteenBitMask = 0b1111111111111111;

// Function prototypes
instruction getInstructionType(char buffer[], int i);
void getW(char buffer[], int j, instruction &inst1);
void getS(char buffer[], int j, instruction &inst1);
void getRM(char buffer[], int j, instruction &inst1);
void getREG(char buffer[], int j, instruction &inst1);
void getDisp(char buffer[], int j, instruction &inst1, DispFlag &d);
void getData(char buffer[], int j, instruction &inst1, DispFlag d);
void getSourceAndDest(instruction &inst1);
int getSize(instruction &inst1);
void printCommand(instruction &inst1, DispFlag d);
void emulateCommand(instruction inst, CPU &cpu, Memory &memory, Flags &flag);
void printOperation(instruction inst1, CPU cpu);
string enumRMToString(RM rm, int d);
string enumSRToString(SR sr);
string enumWToString(WFlag w);
string enumMnemonicToString(Mnemonic m);
int getCPUSlotRM(RM ax, Lo_Hi_Byte &lo);
int getCPUMem(instruction inst1, RM ax, CPU cpu);
int getCPUSlotSR(SR es);
void setFlags(instruction inst, i32 destVal, i32 source, Flags &flag);
void printFlags(Flags &flag);

int main()
{
    ifstream inputFile;

    // open file
    string filename = "C:\\Users\\hjami\\listing_0052_memory_add_loop";
    inputFile.open(filename, ios::in | ios::binary);

    if (!inputFile)
    {
        cout << "Error opening file" << endl;
    }

    // Determine number of bytes in file
    inputFile.seekg(0, ios::end);
    int fileSize = inputFile.tellg();
    inputFile.seekg(0, ios::beg);

    // Create array to store each byte of data and read in data
    char *buffer = new char[fileSize];
    inputFile.read(buffer, fileSize);

    // Create simulated CPU & flags
    CPU registers;
    Flags flag;
    Memory memory;

    // Decompile and print instruction
    while (registers.regSlots[12] < fileSize)
    {
        instruction command = getInstructionType(buffer, registers.regSlots[12]);
        DispFlag d = DispFlag::No_Displacement;
        getW(buffer, registers.regSlots[12], command);
        auto source = RM::not_set;
        auto dest = RM::not_set;
        if (((command.mnemonic == add) || (command.mnemonic == sub) || (command.mnemonic == cmp)) && (command.op_tag == immediate_to_register_mem))
        {
            getS(buffer, registers.regSlots[12], command);
        }
        getRM(buffer, registers.regSlots[12], command);
        getREG(buffer, registers.regSlots[12], command);
        getDisp(buffer, registers.regSlots[12], command, d);
        getData(buffer, registers.regSlots[12], command, d);
        getSourceAndDest(command);

        // Print instruction
        printCommand(command, d);

        int increment = getSize(command);
        registers.regSlots[12] += increment;

        // Perform operation
        emulateCommand(command, registers, memory, flag);
    }

    // Print register states
    cout << endl
         << "Final Registers:" << endl;
    for (int k = 0; k < 13; k++)
    {
        cout << regList[k] << ": " << registers.regSlots[k] << endl; // register ID and value
    }

    // Print flag states
    cout << endl
         << "Final Flags: " << endl;
    for (int l = 0; l < 9; l++)
    {
        cout << flagsList[flagsListMask[l]] << ": " << flag.flags[flagsListMask[l]] << endl;
    }

    delete[] buffer;
    return 0;
}

instruction getInstructionType(char buffer[], int j)
{
    int test1 = (buffer[j] >> 1) & sevBitConv;
    int test2 = (buffer[j] >> 2) & sixBitConv;
    int test4 = (buffer[j] >> 4) & fourBitConv;
    int byteTwoTest = (buffer[j + 1] >> 3) & threeBitconv;
    Operation oper_tag;
    Mnemonic mnemonic;

    switch (buffer[j] & 0b11111111)
    {
    case 0b10001110:
        oper_tag = register_mem_to_from_seg_register;
        mnemonic = mov;
        break;
    case 0b10001100:
        oper_tag = register_mem_to_from_seg_register;
        mnemonic = mov;
        break;
    case 0b01110100:
        oper_tag = conditional_jump;
        mnemonic = je;
        break;
    case 0b01111100:
        oper_tag = conditional_jump;
        mnemonic = jl;
        break;
    case 0b01111110:
        oper_tag = conditional_jump;
        mnemonic = jle;
        break;
    case 0b01110010:
        oper_tag = conditional_jump;
        mnemonic = jb;
        break;
    case 0b01110110:
        oper_tag = conditional_jump;
        mnemonic = jbe;
        break;
    case 0b01111010:
        oper_tag = conditional_jump;
        mnemonic = jp;
        break;
    case 0b01110000:
        oper_tag = conditional_jump;
        mnemonic = jo;
        break;
    case 0b01111000:
        oper_tag = conditional_jump;
        mnemonic = js;
        break;
    case 0b01110101:
        oper_tag = conditional_jump;
        mnemonic = jne;
        break;
    case 0b01111101:
        oper_tag = conditional_jump;
        mnemonic = jnl;
        break;
    case 0b01111111:
        oper_tag = conditional_jump;
        mnemonic = jg;
        break;
    case 0b01110011:
        oper_tag = conditional_jump;
        mnemonic = jnb;
        break;
    case 0b01110111:
        oper_tag = conditional_jump;
        mnemonic = ja;
        break;
    case 0b01111011:
        oper_tag = conditional_jump;
        mnemonic = jnp;
        break;
    case 0b01110001:
        oper_tag = conditional_jump;
        mnemonic = jno;
        break;
    case 0b01111001:
        oper_tag = conditional_jump;
        mnemonic = jns;
        break;
    case 0b11100010:
        oper_tag = conditional_jump;
        mnemonic = loop;
        break;
    case 0b11100001:
        oper_tag = conditional_jump;
        mnemonic = loopz;
        break;
    case 0b11100000:
        oper_tag = conditional_jump;
        mnemonic = loopnz;
        break;
    case 0b11100011:
        oper_tag = conditional_jump;
        mnemonic = jcxz;
        break;
    }

    switch (test1)
    {
    case 0b01100011:
        mnemonic = mov;
        oper_tag = immediate_to_register_mem;
        break;
    case 0b00100000:
        oper_tag = immediate_to_register_mem;
        switch (byteTwoTest)
        {
        case 0b00000101:
            mnemonic = sub;
            break;
        case 0b00000011:
            mnemonic = cmp;
            break;
        }
        break;
    case 0b01010000:
        mnemonic = mov;
        oper_tag = memory_to_acc_or_vv;
        break;
    case 0b00000010:
        mnemonic = add;
        oper_tag = immediate_to_register;
        break;
    case 0b00010110:
        mnemonic = sub;
        oper_tag = immediate_to_register;
        break;
    case 0b00011110:
        mnemonic = cmp;
        oper_tag = immediate_to_register;
        break;
    case 0b01010001:
        mnemonic = mov;
        oper_tag = memory_to_acc_or_vv;
        break;
    }

    switch (test2)
    {
    case 0b00100010:
        mnemonic = mov;
        oper_tag = register_mem_to_from_register;
        break;
    case 0b00000000:
        mnemonic = add;
        oper_tag = register_mem_to_from_register;
        break;
    case 0b00001010:
        mnemonic = sub;
        oper_tag = register_mem_to_from_register;
        break;
    case 0b00001110:
        mnemonic = cmp;
        oper_tag = register_mem_to_from_register;
        break;
    case 0b00100000:
        oper_tag = immediate_to_register_mem;
        switch (byteTwoTest)
        {
        case 0b00000000:
            mnemonic = add;
            break;
        case 0b00000101:
            mnemonic = sub;
            break;
        case 0b00000111:
            mnemonic = cmp;
            break;
        }
        break;
    }

    switch (test4)
    {
    case 0b00001011:
        mnemonic = mov;
        oper_tag = immediate_to_register;
        break;
    }

    instruction inst1(buffer[j], oper_tag, mnemonic);
    inst1.op_tag = oper_tag;
    inst1.op_code = buffer[j];
    inst1.byteTwo = buffer[j + 1];

    if (inst1.op_tag == register_mem_to_from_seg_register)
    {
        switch (buffer[j] & lowBitsMask)
        {
        case 0b10001110:
            inst1.reg_mem_to_from_seg_reg.d = segment_register_is_destination;
            break;
        case 0b10001100:
            inst1.reg_mem_to_from_seg_reg.d = segment_register_is_source;
            break;
        }
    }

    if (inst1.op_tag == memory_to_acc_or_vv)
    {
        switch (test1)
        {
        case 0b01010000:
            inst1.mem_to_acc.d = accumulator_is_destination;
            break;
        case 0b01010001:
            inst1.mem_to_acc.d = accumulator_is_source;
            break;
        }
    }

    if (inst1.op_tag == register_mem_to_from_register)
    {
        int d_value = ((buffer[j] >> 1) & 1);
        switch (d_value)
        {
        case 0:
            inst1.reg_mem_to_from_reg.d = register_is_source;
            break;
        case 1:
            inst1.reg_mem_to_from_reg.d = register_is_destination;
            break;
        }
    }

    return inst1;
}

void getW(char buffer[], int j, instruction &inst1)
{
    int wide = -1;
    switch (inst1.op_tag)
    {
    case immediate_to_register:
        if (inst1.mnemonic == mov)
        {
            wide = ((buffer[j] >> 3) & 1);
        }
        else
        {
            wide = (buffer[j] & 1);
        }
        break;
    case register_mem_to_from_register:
    case immediate_to_register_mem:
    case memory_to_acc_or_vv:
        wide = (buffer[j] & 1);
        break;
    case register_mem_to_from_seg_register:
        wide = 1;
        break;
    }
    if (wide)
    {
        inst1.w = Word;
    }
    else
    {
        inst1.w = Byte;
    }
}

void getS(char buffer[], int j, instruction &inst1)
{
    int sVal = -1;
    if (inst1.op_tag == immediate_to_register_mem)
    {
        if (inst1.w == Word)
        {
            sVal = ((buffer[j] >> 1) & 1);
        }
        else
        {
            sVal = 0;
        }
    }
    inst1.imm_to_reg_mem.s = sVal;
}

void getRM(char buffer[], int j, instruction &inst1)
{
    MOD mode = (MOD)((buffer[j + 1] >> 6) & twoBitConv);
    switch (inst1.op_tag)
    {
    case register_mem_to_from_register:
        inst1.reg_mem_to_from_reg.mod = mode;
        break;
    case immediate_to_register_mem:
        inst1.imm_to_reg_mem.mod = mode;
        break;
    case register_mem_to_from_seg_register:
        inst1.reg_mem_to_from_seg_reg.mod = mode;
        break;
    }

    if (inst1.op_tag == memory_to_acc_or_vv)
    {
        if (inst1.w == Word)
        {
            inst1.mem_to_acc.address = ((buffer[j + 2] >> 8) & 0b1111111100000000) + (buffer[j + 1] & 0b0000000011111111);
        }
        else
        {
            inst1.mem_to_acc.address = buffer[j + 1];
        }
    }
    else
    {
        int rm1 = (buffer[j + 1] & threeBitconv);
        RM rmConv = RM::not_set;
        switch (mode)
        {
        case memory_mode:
            switch (rm1)
            {
            case 0:
                rmConv = RM::bx_plus_si;
                break;
            case 1:
                rmConv = RM::bx_plus_di;
                break;
            case 2:
                rmConv = RM::bp_plus_si;
                break;
            case 3:
                rmConv = RM::bp_plus_di;
                break;
            case 4:
                rmConv = RM::si;
                break;
            case 5:
                rmConv = RM::di;
                break;
            case 6:
                rmConv = RM::direct_address;
                break;
            case 7:
                rmConv = RM::bx;
                break;
            }
            break;
        case memory_mode_8_bit:
            switch (rm1)
            {
            case 0:
                rmConv = RM::bx_plus_si_plus8;
                break;
            case 1:
                rmConv = RM::bx_plus_di_plus8;
                break;
            case 2:
                rmConv = RM::bp_plus_si_plus8;
                break;
            case 3:
                rmConv = RM::bp_plus_di_plus8;
                break;
            case 4:
                rmConv = RM::si_plus8;
                break;
            case 5:
                rmConv = RM::di_plus8;
                break;
            case 6:
                rmConv = RM::bp_plus8;
                break;
            case 7:
                rmConv = RM::bx_plus8;
                break;
            }
            break;
        case memory_mode_16_bit:
            switch (rm1)
            {
            case 0:
                rmConv = RM::bx_plus_si_plus16;
                break;
            case 1:
                rmConv = RM::bx_plus_di_plus16;
                break;
            case 2:
                rmConv = RM::bp_plus_si_plus16;
                break;
            case 3:
                rmConv = RM::bp_plus_di_plus16;
                break;
            case 4:
                rmConv = RM::si_plus16;
                break;
            case 5:
                rmConv = RM::di_plus16;
                break;
            case 6:
                rmConv = RM::bp_plus16;
                break;
            case 7:
                rmConv = RM::bx_plus16;
                break;
            }
            break;
        case register_mode:
            if (inst1.w == Word)
            {
                switch (rm1)
                {
                case 0:
                    rmConv = RM::ax;
                    break;
                case 1:
                    rmConv = RM::cx;
                    break;
                case 2:
                    rmConv = RM::dx;
                    break;
                case 3:
                    rmConv = RM::bx;
                    break;
                case 4:
                    rmConv = RM::sp;
                    break;
                case 5:
                    rmConv = RM::bp;
                    break;
                case 6:
                    rmConv = RM::si;
                    break;
                case 7:
                    rmConv = RM::di;
                    break;
                }
            }
            else if (inst1.w == Byte)
            {
                switch (rm1)
                {
                case 0:
                    rmConv = RM::al;
                    break;
                case 1:
                    rmConv = RM::cl;
                    break;
                case 2:
                    rmConv = RM::dl;
                    break;
                case 3:
                    rmConv = RM::bl;
                    break;
                case 4:
                    rmConv = RM::ah;
                    break;
                case 5:
                    rmConv = RM::ch;
                    break;
                case 6:
                    rmConv = RM::dh;
                    break;
                case 7:
                    rmConv = RM::bh;
                    break;
                }
            }
            break;
        }
        switch (inst1.op_tag)
        {
        case register_mem_to_from_register:
            inst1.reg_mem_to_from_reg.rm = rmConv;
            break;
        case immediate_to_register_mem:
            inst1.imm_to_reg_mem.rm = rmConv;
            break;
        case register_mem_to_from_seg_register:
            inst1.reg_mem_to_from_seg_reg.rm = rmConv;
            break;
        }
    }
}

void getREG(char buffer[], int j, instruction &inst1)
{
    int regist;
    switch (inst1.op_tag)
    {
    case register_mem_to_from_register:
        regist = ((buffer[j + 1] >> 3) & threeBitconv);
        if (inst1.w == Word)
        {
            switch (regist)
            {
            case 0:
                inst1.reg_mem_to_from_reg.reg = RM::ax;
                break;
            case 1:
                inst1.reg_mem_to_from_reg.reg = RM::cx;
                break;
            case 2:
                inst1.reg_mem_to_from_reg.reg = RM::dx;
                break;
            case 3:
                inst1.reg_mem_to_from_reg.reg = RM::bx;
                break;
            case 4:
                inst1.reg_mem_to_from_reg.reg = RM::sp;
                break;
            case 5:
                inst1.reg_mem_to_from_reg.reg = RM::bp;
                break;
            case 6:
                inst1.reg_mem_to_from_reg.reg = RM::si;
                break;
            case 7:
                inst1.reg_mem_to_from_reg.reg = RM::di;
                break;
            }
        }
        else
        {
            switch (regist)
            {
            case 0:
                inst1.reg_mem_to_from_reg.reg = RM::al;
                break;
            case 1:
                inst1.reg_mem_to_from_reg.reg = RM::cl;
                break;
            case 2:
                inst1.reg_mem_to_from_reg.reg = RM::dl;
                break;
            case 3:
                inst1.reg_mem_to_from_reg.reg = RM::bl;
                break;
            case 4:
                inst1.reg_mem_to_from_reg.reg = RM::ah;
                break;
            case 5:
                inst1.reg_mem_to_from_reg.reg = RM::ch;
                break;
            case 6:
                inst1.reg_mem_to_from_reg.reg = RM::dh;
                break;
            case 7:
                inst1.reg_mem_to_from_reg.reg = RM::bh;
                break;
            }
        }
        break;
    case immediate_to_register:
        switch ((inst1.op_code >> 1) & sevBitConv)
        {
        case 0b00000010:
        case 0b00010110:
            if (inst1.w == Word)
            {
                inst1.imm_to_reg.reg = RM::ax;
            }
            else
            {
                inst1.imm_to_reg.reg = RM::al;
            }
            break;
        case 0b00011110:
            inst1.imm_to_reg.reg = RM::al;
            break;
        default:
            regist = buffer[j] & threeBitconv;
            if (inst1.w == Word)
            {
                switch (regist)
                {
                case 0:
                    inst1.imm_to_reg.reg = RM::ax;
                    break;
                case 1:
                    inst1.imm_to_reg.reg = RM::cx;
                    break;
                case 2:
                    inst1.imm_to_reg.reg = RM::dx;
                    break;
                case 3:
                    inst1.imm_to_reg.reg = RM::bx;
                    break;
                case 4:
                    inst1.imm_to_reg.reg = RM::sp;
                    break;
                case 5:
                    inst1.imm_to_reg.reg = RM::bp;
                    break;
                case 6:
                    inst1.imm_to_reg.reg = RM::si;
                    break;
                case 7:
                    inst1.imm_to_reg.reg = RM::di;
                    break;
                }
                break;
            }
            else
            {
                switch (regist)
                {
                case 0:
                    inst1.imm_to_reg.reg = RM::al;
                    break;
                case 1:
                    inst1.imm_to_reg.reg = RM::cl;
                    break;
                case 2:
                    inst1.imm_to_reg.reg = RM::dl;
                    break;
                case 3:
                    inst1.imm_to_reg.reg = RM::bl;
                    break;
                case 4:
                    inst1.imm_to_reg.reg = RM::ah;
                    break;
                case 5:
                    inst1.imm_to_reg.reg = RM::ch;
                    break;
                case 6:
                    inst1.imm_to_reg.reg = RM::dh;
                    break;
                case 7:
                    inst1.imm_to_reg.reg = RM::bh;
                    break;
                }
            }
            break;
        }
        break;
    case register_mem_to_from_seg_register:
        regist = ((buffer[j + 1] >> 3) & twoBitConv);
        switch (regist)
        {
        case 0:
            inst1.reg_mem_to_from_seg_reg.sr = SR::es;
            break;
        case 1:
            inst1.reg_mem_to_from_seg_reg.sr = SR::cs;
            break;
        case 2:
            inst1.reg_mem_to_from_seg_reg.sr = SR::ss;
            break;
        case 3:
            inst1.reg_mem_to_from_seg_reg.sr = SR::ds;
            break;
        }
        break;
    };
}

void getDisp(char buffer[], int j, instruction &inst1, DispFlag &d)
{
    switch (inst1.op_tag)
    {
    case register_mem_to_from_register:
        if (inst1.reg_mem_to_from_reg.mod == memory_mode_8_bit)
        {
            inst1.reg_mem_to_from_reg.disp = buffer[j + 2];
            d = DispFlag::Has_Displacement;
        }
        else if (inst1.reg_mem_to_from_reg.mod == memory_mode_16_bit)
        {
            inst1.reg_mem_to_from_reg.disp = ((buffer[j + 3] << 8) & highBitsMask) + (buffer[j + 2] & lowBitsMask);
            d = DispFlag::Has_Displacement;
        }
        else if ((inst1.reg_mem_to_from_reg.mod == memory_mode) && (inst1.reg_mem_to_from_reg.rm == RM::direct_address))
        {
            inst1.reg_mem_to_from_reg.disp = ((buffer[j + 3] << 8) & highBitsMask) + (buffer[j + 2] & lowBitsMask);
            d = DispFlag::Has_Displacement;
        }
        break;
    case immediate_to_register_mem:
        if (inst1.imm_to_reg_mem.mod == memory_mode_8_bit)
        {
            inst1.imm_to_reg_mem.disp = buffer[j + 2];
            d = DispFlag::Has_Displacement;
        }
        else if (inst1.imm_to_reg_mem.mod == memory_mode_16_bit)
        {
            inst1.imm_to_reg_mem.disp = ((buffer[j + 3] << 8) & highBitsMask) + (buffer[j + 2] & lowBitsMask);
            d = DispFlag::Has_Displacement;
        }
        else if ((inst1.imm_to_reg_mem.mod == memory_mode) && (inst1.imm_to_reg_mem.rm == RM::direct_address))
        {
            inst1.imm_to_reg_mem.disp = ((buffer[j + 3] << 8) & highBitsMask) + (buffer[j + 2] & lowBitsMask);
            d = DispFlag::Has_Displacement;
        }

        break;
    case register_mem_to_from_seg_register:
        if (inst1.reg_mem_to_from_seg_reg.mod == memory_mode_8_bit)
        {
            inst1.reg_mem_to_from_seg_reg.disp = buffer[j + 2];
            d = DispFlag::Has_Displacement;
        }
        else if (inst1.reg_mem_to_from_seg_reg.mod == memory_mode_16_bit)
        {
            inst1.reg_mem_to_from_seg_reg.disp = ((buffer[j + 3] << 8) & highBitsMask) + (buffer[j + 2] & lowBitsMask);
            d = DispFlag::Has_Displacement;
        }
        else if ((inst1.reg_mem_to_from_seg_reg.mod == memory_mode) && (inst1.reg_mem_to_from_seg_reg.rm == RM::direct_address))
        {
            inst1.reg_mem_to_from_seg_reg.disp = ((buffer[j + 3] << 8) & highBitsMask) + (buffer[j + 2] & lowBitsMask);
            d = DispFlag::Has_Displacement;
        }
        break;
    }
}

void getData(char buffer[], int j, instruction &inst1, DispFlag d)
{
    int offset = 0;
    switch (inst1.op_tag)
    {
    case immediate_to_register_mem:
        if (d == DispFlag::No_Displacement)
        {
            offset = 2;
        }
        else
        {
            switch (inst1.imm_to_reg_mem.mod)
            {
            case 0:
                if (inst1.imm_to_reg_mem.rm != RM::direct_address)
                {
                    offset = 2;
                }
                else
                {
                    offset = 4;
                }
                break;
            case 3:
                offset = 2;
                break;
            case 1:
                offset = 3;
                break;
            case 2:
                offset = 4;
                break;
            }
        }
        if (inst1.w == Word)
        {
            if ((inst1.mnemonic == add) || (inst1.mnemonic == sub) || (inst1.mnemonic == cmp))
            {
                if (inst1.imm_to_reg_mem.s == 0)
                {
                    inst1.imm_to_reg_mem.data = (static_cast<int16_t>((buffer[j + offset + 1] << 8) & highBitsMask) + (buffer[j + offset] & lowBitsMask));
                }
                else
                {
                    inst1.imm_to_reg_mem.data = static_cast<int16_t>(buffer[j + offset]);
                }
            }
            else
            {
                inst1.imm_to_reg_mem.data = (static_cast<int16_t>((buffer[j + offset + 1] << 8) & highBitsMask) + (buffer[j + offset] & lowBitsMask));
            }
        }
        else
        {
            inst1.imm_to_reg_mem.data = static_cast<int16_t>(buffer[j + offset]);
        }
        break;
    case immediate_to_register:
        if (inst1.w == Word)
        {
            inst1.imm_to_reg.data = (static_cast<int16_t>((buffer[j + 2] << 8) & highBitsMask) + (buffer[j + 1] & lowBitsMask));
        }
        else
        {
            inst1.imm_to_reg.data = static_cast<int16_t>(buffer[j + 1]);
        }
        break;
    case conditional_jump:
        inst1.cond_jmp.data = static_cast<int16_t>(buffer[j + 1]);
        break;
    }
}

int getSize(instruction &inst1)
{
    bool sMnemonic = (inst1.mnemonic == add) || (inst1.mnemonic == sub) || (inst1.mnemonic == cmp);
    switch (inst1.op_tag)
    {
    case register_mem_to_from_register:
        switch (inst1.reg_mem_to_from_reg.mod)
        {
        case 0:
            if (inst1.reg_mem_to_from_reg.rm != RM::direct_address)
            {
                return 2;
            }
            else
            {
                return 4;
            }
            break;
        case 3:
            return 2;
            break;
        case 1:
            return 3;
            break;
        case 2:
            return 4;
            break;
        }
        break;
    case immediate_to_register_mem:
        switch (inst1.imm_to_reg_mem.mod)
        {
        case 0:
            if (inst1.imm_to_reg_mem.rm != RM::direct_address)
            {
                switch (inst1.w)
                {
                case Word:
                    if (sMnemonic)
                    {
                        if (inst1.imm_to_reg_mem.s == 0)
                        {
                            return 4;
                        }
                        else
                        {
                            return 3;
                        }
                    }
                    else
                    {
                        return 4;
                    }
                case Byte:
                    return 3;
                    break;
                }
            }
            else
            {
                switch (inst1.w)
                {
                case Word:
                    if (sMnemonic)
                    {
                        if (inst1.imm_to_reg_mem.s == 0)
                        {
                            return 6;
                        }
                        else
                        {
                            return 5;
                        }
                    }
                    else
                    {
                        return 6;
                    }
                    break;
                case Byte:
                    return 5;
                    break;
                }
                break;
            }
            break;
        case 3:
            switch (inst1.w)
            {
            case Word:
                if (sMnemonic)
                {
                    if (inst1.imm_to_reg_mem.s == 0)
                    {
                        return 4;
                    }
                    else
                    {
                        return 3;
                    }
                }
                else
                {
                    return 4;
                }
                break;
            case Byte:
                return 3;
                break;
            }
            break;
        case 1:
            switch (inst1.w)
            {
            case Word:
                if (sMnemonic)
                {
                    if (inst1.imm_to_reg_mem.s == 0)
                    {
                        return 5;
                    }
                    else
                    {
                        return 4;
                    }
                }
                else
                {
                    return 5;
                }
                break;
            case Byte:
                return 4;
                break;
            }
            break;
        case 2:
            switch (inst1.w)
            {
            case Word:
                if (sMnemonic)
                {
                    if (inst1.imm_to_reg_mem.s == 0)
                    {
                        return 6;
                    }
                    else
                    {
                        return 5;
                    }
                }
                else
                {
                    return 6;
                }
                break;
            case Byte:
                return 5;
                break;
            }
            break;
        }
        break;
    case immediate_to_register:
    case memory_to_acc_or_vv:
        switch (inst1.w)
        {
        case Word:
            return 3;
            break;
        case Byte:
            return 2;
            break;
        }
        break;
    case register_mem_to_from_seg_register:
        switch (inst1.reg_mem_to_from_seg_reg.mod)
        {
        case memory_mode:
            if (inst1.reg_mem_to_from_seg_reg.rm != RM::direct_address)
            {
                return 2;
            }
            else
            {
                return 4;
            }
            break;
        case register_mode:
            return 2;
            break;
        case memory_mode_8_bit:
            return 3;
            break;
        case memory_mode_16_bit:
            return 4;
            break;
        }
        break;
    case conditional_jump:
        return 2;
        break;
    }
    cout << "Invalid Optag" << endl;
    exit(1);
}

void getSourceAndDest(instruction &inst1)
{
    switch (inst1.op_tag)
    {
    case register_mem_to_from_register:
        if (inst1.reg_mem_to_from_reg.d == register_is_source)
        {
            inst1.reg_mem_to_from_reg.source = inst1.reg_mem_to_from_reg.reg;
            inst1.reg_mem_to_from_reg.dest = inst1.reg_mem_to_from_reg.rm;
        }
        else if (inst1.reg_mem_to_from_reg.d == register_is_destination)
        {
            inst1.reg_mem_to_from_reg.source = inst1.reg_mem_to_from_reg.rm;
            inst1.reg_mem_to_from_reg.dest = inst1.reg_mem_to_from_reg.reg;
        }
        break;
    case immediate_to_register:
        inst1.imm_to_reg.source = inst1.imm_to_reg.data;
        inst1.imm_to_reg.dest = inst1.imm_to_reg.reg;
        break;
    case immediate_to_register_mem:
        inst1.imm_to_reg_mem.source = inst1.imm_to_reg_mem.data;
        inst1.imm_to_reg_mem.dest = inst1.imm_to_reg_mem.rm;
        break;
    case memory_to_acc_or_vv:
        inst1.mem_to_acc.operandTwo = inst1.mem_to_acc.address;
        inst1.mem_to_acc.operandOne = RM::ax;

        break;
    case register_mem_to_from_seg_register:
        inst1.reg_mem_to_from_seg_reg.operandOne = inst1.reg_mem_to_from_seg_reg.rm;
        inst1.reg_mem_to_from_seg_reg.operandTwo = inst1.reg_mem_to_from_seg_reg.sr;
        break;
    case conditional_jump:
    default:
        break;
    }
}

void printCommand(instruction &inst1, DispFlag d)
{
    string source, dest;
    switch (inst1.op_tag)
    {
    case register_mem_to_from_register:
        if (inst1.reg_mem_to_from_reg.d == register_is_source)
        {
            source = enumRMToString(inst1.reg_mem_to_from_reg.reg, inst1.reg_mem_to_from_reg.disp);
            if ((d == DispFlag::Has_Displacement) && (inst1.reg_mem_to_from_reg.disp != 0))
            {
                dest = " " + enumRMToString(inst1.reg_mem_to_from_reg.rm, inst1.reg_mem_to_from_reg.disp) + " " + to_string(inst1.reg_mem_to_from_reg.disp);
            }
            else
            {
                dest = " " + enumRMToString(inst1.reg_mem_to_from_reg.rm, inst1.reg_mem_to_from_reg.disp);
            }
        }
        else if (inst1.reg_mem_to_from_reg.d == register_is_destination)
        {
            if ((d == DispFlag::Has_Displacement) && (inst1.reg_mem_to_from_reg.disp != 0))
            {
                source = enumRMToString(inst1.reg_mem_to_from_reg.rm, inst1.reg_mem_to_from_reg.disp) + " " + to_string(inst1.reg_mem_to_from_reg.disp);
            }
            else
            {
                source = enumRMToString(inst1.reg_mem_to_from_reg.rm, inst1.reg_mem_to_from_reg.disp);
            }

            dest = " " + enumRMToString(inst1.reg_mem_to_from_reg.reg, inst1.reg_mem_to_from_reg.disp);
        }
        break;
    case immediate_to_register:
        source = to_string(inst1.imm_to_reg.data);
        dest = " " + enumRMToString(inst1.imm_to_reg.reg, 0);
        break;
    case immediate_to_register_mem:
        source = to_string(inst1.imm_to_reg_mem.data);
        if ((d == DispFlag::Has_Displacement) && (inst1.imm_to_reg_mem.disp != 0))
        {
            dest = " " + enumRMToString(inst1.imm_to_reg_mem.rm, inst1.imm_to_reg_mem.disp) + " " + to_string(inst1.imm_to_reg_mem.disp);
        }
        else
        {
            dest = " " + enumRMToString(inst1.imm_to_reg_mem.rm, inst1.imm_to_reg_mem.disp);
        }

        break;
    case memory_to_acc_or_vv:
        if (inst1.mem_to_acc.d == accumulator_is_destination)
        {
            source = to_string(inst1.mem_to_acc.address);
            dest = " ax";
        }
        else if (inst1.mem_to_acc.d == accumulator_is_source)
        {
            source = "ax";
            dest = " " + to_string(inst1.mem_to_acc.address);
        }
        break;
    case register_mem_to_from_seg_register:
        if (inst1.reg_mem_to_from_seg_reg.d == segment_register_is_destination)
        {
            if ((d == DispFlag::Has_Displacement) && (inst1.reg_mem_to_from_seg_reg.disp != 0))
            {
                source = enumRMToString(inst1.reg_mem_to_from_seg_reg.rm, inst1.reg_mem_to_from_seg_reg.disp) + " " + to_string(inst1.reg_mem_to_from_seg_reg.disp);
            }
            else
            {
                source = enumRMToString(inst1.reg_mem_to_from_seg_reg.rm, inst1.reg_mem_to_from_seg_reg.disp);
            }

            dest = " " + enumSRToString(inst1.reg_mem_to_from_seg_reg.sr);
        }
        else if (inst1.reg_mem_to_from_seg_reg.d == segment_register_is_source)
        {
            source = enumSRToString(inst1.reg_mem_to_from_seg_reg.sr);
            if ((d == DispFlag::Has_Displacement) && (inst1.reg_mem_to_from_seg_reg.disp != 0))
            {
                dest = " " + enumRMToString(inst1.reg_mem_to_from_seg_reg.rm, inst1.reg_mem_to_from_seg_reg.disp) + " " + to_string(inst1.reg_mem_to_from_seg_reg.disp);
            }
            else
            {
                dest = " " + enumRMToString(inst1.reg_mem_to_from_seg_reg.rm, inst1.reg_mem_to_from_seg_reg.disp);
            }
        }
        break;
    case conditional_jump:
        source = to_string(inst1.cond_jmp.data);
        dest = "";
        break;
    }
    cout << enumMnemonicToString(inst1.mnemonic) << dest << ", " << source << endl;
}

void emulateCommand(instruction inst1, CPU &cpu, Memory &memory, Flags &flag)
{
    Lo_Hi_Byte levelSource = neither;
    Lo_Hi_Byte levelDest = neither;
    i16 source = 0;
    i16 destination = 0;
    i32 result = 0;
    int destCalc = 0;
    switch (inst1.mnemonic)
    {
    case mov:
        switch (inst1.op_tag)
        {
        case immediate_to_register:
            if (inst1.w == Word)
            {
                cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg.dest, levelDest)] = inst1.imm_to_reg.data;
            }
            else
            {
                destination = cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg.dest, levelDest)];
                source = (inst1.imm_to_reg.data & lowBitsMask);
                if (levelDest == low_byte)
                {
                    cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg.dest, levelDest)] = (destination & highBitsMask) + source;
                }
                else
                {
                    cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg.dest, levelDest)] = (destination & lowBitsMask) + (source << 8);
                }
            }
            break;
        case immediate_to_register_mem:
            source = inst1.imm_to_reg_mem.source;
            switch (inst1.imm_to_reg_mem.mod)
            {
            case memory_mode:
            case memory_mode_8_bit:
            case memory_mode_16_bit:
                destCalc = getCPUMem(inst1, inst1.imm_to_reg_mem.dest, cpu);
                if (inst1.w == Word)
                {
                    memory.memSlots[destCalc] = (inst1.imm_to_reg_mem.data & lowBitsMask);
                    memory.memSlots[destCalc + 1] = (inst1.imm_to_reg_mem.data & highBitsMask);
                }
                else
                {
                    memory.memSlots[destCalc] = inst1.imm_to_reg_mem.data;
                }
                break;
            case register_mode:
                if (inst1.w == Word)
                {
                    cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg_mem.dest, levelDest)] = inst1.imm_to_reg_mem.data;
                }
                else
                {
                    destination = cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg_mem.dest, levelDest)];
                    source = inst1.imm_to_reg_mem.data;
                    if (levelDest == low_byte)
                    {
                        cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg_mem.dest, levelDest)] = (destination & highBitsMask) + source;
                    }
                    else
                    {
                        cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg_mem.dest, levelDest)] = (destination & lowBitsMask) + (source << 8);
                    }
                }
                break;
            }
            break;
        case register_mem_to_from_register:
            if (inst1.reg_mem_to_from_reg.d == register_is_source)
            {
                source = cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.source, levelSource)];
                destCalc = getCPUMem(inst1, inst1.reg_mem_to_from_reg.dest, cpu);
            }
            else
            {
                source = getCPUMem(inst1, inst1.reg_mem_to_from_reg.source, cpu);
                destination = getCPUSlotRM(inst1.reg_mem_to_from_reg.dest, levelDest);
            }
            switch (inst1.reg_mem_to_from_reg.mod)
            {
            case memory_mode:
            case memory_mode_8_bit:
            case memory_mode_16_bit:
                if (inst1.reg_mem_to_from_reg.d == register_is_source)
                {
                    if (inst1.w == Word)
                    {
                        memory.memSlots[destCalc] = (source & lowBitsMask);
                        memory.memSlots[destCalc + 1] = (source & highBitsMask);
                    }
                    else
                    {
                        memory.memSlots[destCalc] = source;
                    }
                }
                else
                {
                    if (inst1.w == Word)
                    {
                        cpu.regSlots[destination] = (memory.memSlots[source + 1] & highBitsMask) + (memory.memSlots[source] & lowBitsMask);
                    }
                    else
                    {
                        cpu.regSlots[destination] = memory.memSlots[source];
                    }
                }
                break;
            case register_mode:
                if (inst1.w == Word)
                {
                    cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.dest, levelSource)] = cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.source, levelSource)];
                }
                else
                {
                    source = cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.source, levelSource)];
                    destination = cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.dest, levelDest)];
                    i16 tempSource = -1;
                    i16 tempDest = -1;
                    if (levelSource == low_byte)
                    {
                        tempSource = source & lowBitsMask;
                        if (levelDest == low_byte)
                        {
                            cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.dest, levelDest)] = (destination & highBitsMask) + tempSource;
                        }
                        else if (levelDest == high_byte)
                        {
                            cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.dest, levelDest)] = (destination & lowBitsMask) + (tempSource << 8);
                        }
                    }
                    else if (levelSource == high_byte)
                    {
                        tempSource = ((source & highBitsMask) >> 8);
                        if (levelDest == low_byte)
                        {
                            cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.dest, levelDest)] = (destination & highBitsMask) + tempSource;
                        }
                        else if (levelDest == high_byte)
                        {
                            cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.dest, levelDest)] = (destination & lowBitsMask) + (tempSource << 8);
                        }
                    }
                }
                break;
            }
            break;
        case register_mem_to_from_seg_register:
            switch (inst1.reg_mem_to_from_seg_reg.mod)
            {
            case memory_mode: // Not yet coded
                break;
            case memory_mode_8_bit: // Not yet coded
                break;
            case memory_mode_16_bit: // Not yet coded
                break;
            case register_mode:
                if (inst1.reg_mem_to_from_seg_reg.d == segment_register_is_destination)
                {
                    cpu.regSlots[getCPUSlotSR(inst1.reg_mem_to_from_seg_reg.operandTwo)] = cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_seg_reg.operandOne, levelSource)];
                }
                else
                {
                    cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_seg_reg.operandOne, levelSource)] = cpu.regSlots[getCPUSlotSR(inst1.reg_mem_to_from_seg_reg.operandTwo)];
                }
                break;
            }
            break;
        case memory_to_acc_or_vv: // Not yet coded
            break;
        }
        break;
    case add:
        switch (inst1.op_tag)
        {
        case register_mem_to_from_register:
            source = cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.source, levelSource)];
            destination = cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.dest, levelDest)];
            switch (inst1.reg_mem_to_from_reg.mod)
            {
            case memory_mode: // Not yet coded
                break;
            case memory_mode_8_bit: // Not yet coded
                break;
            case memory_mode_16_bit: // Not yet coded
                break;
            case register_mode:
                if (inst1.w == Word)
                {
                    cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.dest, levelSource)] += cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.source, levelSource)];
                    result = cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.dest, levelSource)] + cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.source, levelSource)];
                }
                else
                {
                    if (levelSource == low_byte)
                    {
                        source = source & lowBitsMask;
                        if (levelDest == low_byte)
                        {
                            cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.dest, levelDest)] = (destination & highBitsMask) + ((source + (destination & lowBitsMask)) & lowBitsMask);
                            result = (destination & highBitsMask) + ((source + (destination & lowBitsMask)) & lowBitsMask);
                        }
                        else if (levelDest == high_byte)
                        {
                            cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.dest, levelDest)] = (destination & lowBitsMask) + ((destination & highBitsMask) + (source << 8));
                            result = (destination & lowBitsMask) + ((destination & highBitsMask) + (source << 8));
                        }
                    }
                    else if (levelSource == high_byte)
                    {
                        source = ((source & highBitsMask) >> 8);
                        if (levelDest == low_byte)
                        {
                            cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.dest, levelDest)] = (destination & highBitsMask) + (((destination & lowBitsMask) + source) & lowBitsMask);
                            result = (destination & highBitsMask) + (((destination & lowBitsMask) + source) & lowBitsMask);
                        }
                        else if (levelDest == high_byte)
                        {
                            cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.dest, levelDest)] = (destination & lowBitsMask) + ((destination & highBitsMask) + (source << 8));
                            result = (destination & lowBitsMask) + ((destination & highBitsMask) + (source << 8));
                        }
                    }
                }
                break;
            }
            setFlags(inst1, result, source, flag);
            break;
        case immediate_to_register_mem:
            switch (inst1.imm_to_reg_mem.mod)
            {
            case memory_mode: // Not yet coded
                break;
            case memory_mode_8_bit: // Not yet coded
                break;
            case memory_mode_16_bit: // Not yet coded
                break;
            case register_mode:
                destination = cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg_mem.dest, levelDest)];
                source = inst1.imm_to_reg_mem.data;
                if (inst1.w == Word)
                {
                    cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg_mem.dest, levelDest)] += source;
                    result = destination + source;
                }
                else
                {
                    if (levelDest == low_byte)
                    {
                        cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg_mem.dest, levelDest)] = ((destination & highBitsMask) + (source + (destination & lowBitsMask)));
                        result = ((destination & highBitsMask) + (source + (destination & lowBitsMask)));
                    }
                    else
                    {
                        cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg_mem.dest, levelDest)] = (destination & lowBitsMask) + ((destination & highBitsMask) + (source << 8));
                        result = (destination & lowBitsMask) + ((destination & highBitsMask) + (source << 8));
                    }
                }
                setFlags(inst1, result, source, flag);
                break;
            }
            break;
        case immediate_to_register:
            destination = cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg.dest, levelDest)];
            source = inst1.imm_to_reg.data;
            if (inst1.w == Word)
            {
                cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg.dest, levelDest)] += inst1.imm_to_reg.data;
                result = destination + inst1.imm_to_reg.data;
            }
            else
            {
                source = (inst1.imm_to_reg.data & lowBitsMask);
                if (levelDest == low_byte)
                {
                    cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg.dest, levelDest)] = (destination & highBitsMask) + ((destination & lowBitsMask) + source);
                    result = (destination & highBitsMask) + ((destination & lowBitsMask) + source);
                }
                else
                {
                    cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg.dest, levelDest)] = (destination & lowBitsMask) + ((destination & highBitsMask) + (source << 8));
                    result = (destination & lowBitsMask) + ((destination & highBitsMask) + (source << 8));
                }
            }
            setFlags(inst1, result, source, flag);
            break;
        }
        break;
    case sub:
        switch (inst1.op_tag)
        {
        case register_mem_to_from_register:
            switch (inst1.reg_mem_to_from_reg.mod)
            {
            case memory_mode: // Not yet coded
                break;
            case memory_mode_8_bit: // Not yet coded
                break;
            case memory_mode_16_bit: // Not yet coded
                break;
            case register_mode:
                source = cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.source, levelSource)];
                destination = cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.dest, levelDest)];
                if (inst1.w == Word)
                {
                    cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.dest, levelSource)] -= source;
                    result = destination - source;
                }
                else
                {
                    if (levelSource == low_byte)
                    {
                        source = source & lowBitsMask;
                        if (levelDest == low_byte)
                        {
                            cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.dest, levelDest)] = (destination & highBitsMask) + (((destination & lowBitsMask) - source) & lowBitsMask);
                            result = (destination & highBitsMask) + (((destination & lowBitsMask) - source) & lowBitsMask);
                        }
                        else if (levelDest == high_byte)
                        {
                            cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.dest, levelDest)] = (destination & lowBitsMask) + ((destination & highBitsMask) - (source << 8));
                            result = (destination & lowBitsMask) + ((destination & highBitsMask) - (source << 8));
                        }
                    }
                    else if (levelSource == high_byte)
                    {
                        source = ((source & highBitsMask) >> 8);
                        if (levelDest == low_byte)
                        {
                            cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.dest, levelDest)] = (destination & highBitsMask) + (((destination & lowBitsMask) - source) & lowBitsMask);
                            result = (destination & highBitsMask) + (((destination & lowBitsMask) - source) & lowBitsMask);
                        }
                        else if (levelDest == high_byte)
                        {
                            cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.dest, levelDest)] = (destination & lowBitsMask) + ((destination & highBitsMask) - (source << 8));
                            result = (destination & lowBitsMask) + ((destination & highBitsMask) - (source << 8));
                        }
                    }
                }
                break;
            }
            setFlags(inst1, result, source, flag);
            break;
        case immediate_to_register_mem:
            switch (inst1.imm_to_reg_mem.mod)
            {
            case memory_mode: // Not yet coded
                break;
            case memory_mode_8_bit: // Not yet coded
                break;
            case memory_mode_16_bit: // Not yet coded
                break;
            case register_mode:
                destination = cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg_mem.dest, levelDest)];
                source = inst1.imm_to_reg_mem.data;
                if (inst1.w == Word)
                {
                    cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg_mem.dest, levelDest)] -= source;
                    result = destination - source;
                }
                else
                {
                    if (levelDest == low_byte)
                    {
                        cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg_mem.dest, levelDest)] = ((destination & highBitsMask) + ((destination & lowBitsMask) - source));
                        result = ((destination & highBitsMask) + ((destination & lowBitsMask) - source));
                    }
                    else
                    {
                        cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg_mem.dest, levelDest)] = (destination & lowBitsMask) + ((destination & highBitsMask) - (source << 8));
                        result = (destination & lowBitsMask) + ((destination & highBitsMask) - (source << 8));
                    }
                }
                break;
            }
            setFlags(inst1, result, source, flag);
            break;
        case immediate_to_register:
            destination = cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg.dest, levelDest)];
            source = inst1.imm_to_reg.data;
            if (inst1.w == Word)
            {
                cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg.dest, levelDest)] -= inst1.imm_to_reg.data;
                result = destination - inst1.imm_to_reg.data;
            }
            else
            {
                source = (inst1.imm_to_reg.data & lowBitsMask);
                if (levelDest == low_byte)
                {
                    cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg.dest, levelDest)] = (destination & highBitsMask) + ((destination & lowBitsMask) - source);
                    result = (destination & highBitsMask) + ((destination & lowBitsMask) - source);
                }
                else
                {
                    cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg.dest, levelDest)] = (destination & lowBitsMask) + ((destination & highBitsMask) - (source << 8));
                    result = (destination & lowBitsMask) + ((destination & highBitsMask) - (source << 8));
                }
            }
            setFlags(inst1, result, source, flag);
            break;
        }
        break;
    case cmp:
        switch (inst1.op_tag)
        {
        case register_mem_to_from_register:
            switch (inst1.reg_mem_to_from_reg.mod)
            {
            case memory_mode: // Not yet coded
                break;
            case memory_mode_8_bit: // Not yet coded
                break;
            case memory_mode_16_bit: // Not yet coded
                break;
            case register_mode:
                source = cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.source, levelSource)];
                destination = cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.dest, levelDest)];
                if (inst1.w == Word)
                {
                    result = destination - cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.source, levelSource)];
                }
                else
                {
                    if (levelSource == low_byte)
                    {
                        source = source & lowBitsMask;
                        if (levelDest == low_byte)
                        {
                            result = (destination & highBitsMask) + (((destination & lowBitsMask) - source) & lowBitsMask);
                        }
                        else if (levelDest == high_byte)
                        {
                            result = (destination & lowBitsMask) + ((destination & highBitsMask) - (source << 8));
                        }
                    }
                    else if (levelSource == high_byte)
                    {
                        source = ((source & highBitsMask) >> 8);
                        if (levelDest == low_byte)
                        {
                            result = (destination & highBitsMask) + (((destination & lowBitsMask) - source) & lowBitsMask);
                        }
                        else if (levelDest == high_byte)
                        {
                            result = (destination & lowBitsMask) + ((destination & highBitsMask) - (source << 8));
                        }
                    }
                }
                setFlags(inst1, result, source, flag);
                break;
            }
        case immediate_to_register_mem:
            switch (inst1.imm_to_reg_mem.mod)
            {
            case memory_mode: // Not yet coded
                break;
            case memory_mode_8_bit: // Not yet coded
                break;
            case memory_mode_16_bit: // Not yet coded
                break;
            case register_mode:
                destination = cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg_mem.dest, levelDest)];
                source = inst1.imm_to_reg_mem.data;
                if (inst1.w == Word)
                {
                    result = destination - inst1.imm_to_reg_mem.data;
                }
                else
                {
                    if (levelDest == low_byte)
                    {
                        result = ((destination & highBitsMask) + ((destination & lowBitsMask) - source));
                    }
                    else
                    {
                        result = (destination & lowBitsMask) + ((destination & highBitsMask) - (source << 8));
                    }
                }
                setFlags(inst1, result, source, flag);
                break;
            }
            break;
        case immediate_to_register:
            destination = cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg.dest, levelDest)];
            source = (inst1.imm_to_reg.data & lowBitsMask);
            if (inst1.w == Word)
            {
                result = destination - inst1.imm_to_reg.data;
            }
            else
            {
                if (levelDest == low_byte)
                {
                    result = (destination & highBitsMask) + ((destination & lowBitsMask) - source);
                }
                else
                {
                    result = (destination & lowBitsMask) + ((destination & highBitsMask) - (source << 8));
                }
            }
            setFlags(inst1, result, source, flag);
            break;
        }
        break;
    case jb:
        if (flag.flags[15])
        {
            cpu.regSlots[12] += inst1.cond_jmp.data;
        }
        break;
    case je:
        if (flag.flags[9])
        {
            cpu.regSlots[12] += inst1.cond_jmp.data;
        }
        break;
    case jne:
        if (flag.flags[9] == false)
        {
            cpu.regSlots[12] += inst1.cond_jmp.data;
        }
        break;
    case jp:
        if (flag.flags[13])
        {
            cpu.regSlots[12] += inst1.cond_jmp.data;
        }
        break;
    case loopnz:
        cpu.regSlots[2] -= 1;
        if ((flag.flags[9] == false) && (cpu.regSlots[2] != 0))
        {
            cpu.regSlots[12] += inst1.cond_jmp.data;
        }
        break;
    default: // Other commands not yet emulated
        break;
    }
}

void printOperation(instruction inst1, CPU cpu)
{
    Lo_Hi_Byte junk = neither;
    switch (inst1.op_tag)
    {
    case immediate_to_register:
        cout << regList[getCPUSlotRM(inst1.imm_to_reg.dest, junk)] << " new value is: " << cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg.dest, junk)] << endl;
        break;
    case immediate_to_register_mem:
        cout << regList[getCPUSlotRM(inst1.imm_to_reg_mem.dest, junk)] << " new value is: " << cpu.regSlots[getCPUSlotRM(inst1.imm_to_reg_mem.dest, junk)] << endl;
        break;
    case register_mem_to_from_register:
        cout << regList[getCPUSlotRM(inst1.reg_mem_to_from_reg.dest, junk)] << " new value is: " << cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_reg.dest, junk)] << endl;
        break;
    case register_mem_to_from_seg_register:
        if (inst1.reg_mem_to_from_seg_reg.d == Direction::segment_register_is_destination)
        {
            cout << regList[getCPUSlotSR(inst1.reg_mem_to_from_seg_reg.operandTwo)] << " new value is: " << cpu.regSlots[getCPUSlotSR(inst1.reg_mem_to_from_seg_reg.operandTwo)] << endl;
        }
        else
        {
            cout << regList[getCPUSlotRM(inst1.reg_mem_to_from_seg_reg.operandOne, junk)] << " new value is: " << cpu.regSlots[getCPUSlotRM(inst1.reg_mem_to_from_seg_reg.operandOne, junk)] << endl;
        }
        break;
    case memory_to_acc_or_vv:
        if (inst1.mem_to_acc.d == Direction::accumulator_is_destination)
        {
            cout << regList[getCPUSlotRM(inst1.mem_to_acc.operandOne, junk)] << " new value is: " << cpu.regSlots[getCPUSlotRM(inst1.mem_to_acc.operandOne, junk)] << endl;
        }
        else
        {
            cout << "Not yet coded - ERROR" << endl;
        }
        break;
    }
}

string enumRMToString(RM a, int d)
{
    switch (a)
    {
    case RM::ah:
        return "ah";
    case RM::al:
        return "al";
    case RM::ax:
        return "ax";
    case RM::bh:
        return "bh";
    case RM::bl:
        return "bl";
    case RM::bx:
        return "bx";
    case RM::ch:
        return "ch";
    case RM::cl:
        return "cl";
    case RM::cx:
        return "cx";
    case RM::dh:
        return "dh";
    case RM::dx:
        return "dx";
    case RM::di:
        return "di";
    case RM::si:
        return "si";
    case RM::sp:
        return "sp";
    case RM::bp:
        return "bp";
    case RM::bx_plus_si:
        return "bx + si";
    case RM::bx_plus_di:
        return "bx + di";
    case RM::bp_plus_si:
        return "bp + si";
    case RM::bp_plus_di:
        return "bp + di";
    case RM::direct_address:
        return "";
    case RM::bx_plus_si_plus8:
    case RM::bx_plus_si_plus16:
        if (d <= 0)
        {
            return "bx + si";
        }
        else
        {
            return "bx + si +";
        }
    case RM::bx_plus_di_plus8:
    case RM::bx_plus_di_plus16:
        if (d <= 0)
        {
            return "bx + di";
        }
        else
        {
            return "bx + di +";
        }
    case RM::bp_plus_si_plus8:
    case RM::bp_plus_si_plus16:
        if (d <= 0)
        {
            return "bp + si";
        }
        else
        {
            return "bp + si +";
        }
    case RM::bp_plus_di_plus8:
    case RM::bp_plus_di_plus16:
        if (d <= 0)
        {
            return "bp + di";
        }
        else
        {
            return "bp + di +";
        }
    case RM::si_plus8:
    case RM::si_plus16:
        if (d <= 0)
        {
            return "si";
        }
        else
        {
            return "si +";
        }
    case RM::di_plus8:
    case RM::di_plus16:
        if (d <= 0)
        {
            return "di";
        }
        else
        {
            return "di +";
        }
    case RM::bp_plus8:
    case RM::bp_plus16:
        if (d <= 0)
        {
            return "bp";
        }
        else
        {
            return "bp +";
        }
    case RM::bx_plus8:
    case RM::bx_plus16:
        if (d <= 0)
        {
            return "bx";
        }
        else
        {
            return "bx +";
        }
    default:
        return "unknown";
    }
}

string enumWToString(WFlag b)
{
    switch (b)
    {
    case WFlag::Byte:
        return "byte";
    case WFlag::Not_specified:
        return "not specified";
    case WFlag::Word:
        return "word";
    default:
        return "unknown";
    }
}

string enumSRToString(SR c)
{
    switch (c)
    {
    case SR::cs:
        return "cs";
    case SR::ds:
        return "ds";
    case SR::es:
        return "es";
    case SR::ss:
        return "ss";
    default:
        return "unknown";
    }
}

string enumMnemonicToString(Mnemonic m)
{
    switch (m)
    {
    case add:
        return "add";
    case cmp:
        return "cmp";
    case je:
        return "je";
    case jl:
        return "jl";
    case jle:
        return "jle";
    case jb:
        return "jb";
    case jbe:
        return "jbe";
    case jp:
        return "jp";
    case jo:
        return "jo";
    case js:
        return "js";
    case jne:
        return "jne";
    case jnl:
        return "jnl";
    case jg:
        return "jg";
    case jnb:
        return "jnb";
    case ja:
        return "ja";
    case jnp:
        return "jnp";
    case jno:
        return "jno";
    case jns:
        return "jns";
    case loop:
        return "loop";
    case loopz:
        return "loopz";
    case loopnz:
        return "loopnz";
    case jcxz:
        return "jcxz";
    case mov:
        return "mov";
    case sub:
        return "sub";
    default:
        return "ERROR";
    }
}

int getCPUSlotRM(RM ax, Lo_Hi_Byte &lo)
{
    switch (ax)
    {
    case RM::ax:
        lo = neither;
        return 0;
        break;
    case RM::ah:
        lo = high_byte;
        return 0;
        break;
    case RM::al:
        lo = low_byte;
        return 0;
        break;
    case RM::bx:
        lo = neither;
        return 1;
        break;
    case RM::bh:
        lo = high_byte;
        return 1;
        break;
    case RM::bl:
        lo = low_byte;
        return 1;
        break;
    case RM::cx:
        lo = neither;
        return 2;
        break;
    case RM::ch:
        lo = high_byte;
        return 2;
        break;
    case RM::cl:
        lo = low_byte;
        return 2;
        break;
    case RM::dx:
        lo = neither;
        return 3;
        break;
    case RM::dh:
        lo = high_byte;
        return 3;
        break;
    case RM::dl:
        lo = low_byte;
        return 3;
        break;
    case RM::sp:
        return 4;
        break;
    case RM::bp:
        return 5;
        break;
    case RM::si:
        return 6;
        break;
    case RM::di:
        return 7;
        break;
    default:
        return -1;
        break;
    }
}

int getCPUMem(instruction inst1, RM ax, CPU cpu)
{
    int operand1 = -1;
    int operand2 = -1;
    int operand3 = -1;
    switch (ax)
    {
    case RM::bx_plus_si:
        operand1 = cpu.regSlots[1];
        operand2 = cpu.regSlots[6];
        return operand1 + operand2;
        break;
    case RM::bx_plus_di:
        operand1 = cpu.regSlots[1];
        operand2 = cpu.regSlots[7];
        return operand1 + operand2;
        break;
    case RM::bp_plus_si:
        operand1 = cpu.regSlots[5];
        operand2 = cpu.regSlots[6];
        return operand1 + operand2;
        break;
    case RM::bp_plus_di:
        operand1 = cpu.regSlots[5];
        operand2 = cpu.regSlots[7];
        return operand1 + operand2;
        break;
    case RM::si:
        return cpu.regSlots[6];
        break;
    case RM::di:
        return cpu.regSlots[7];
        break;
    case RM::bx:
        return cpu.regSlots[1];
        break;
    case RM::direct_address:
        switch (inst1.op_tag)
        {
        case register_mem_to_from_register:
            return inst1.reg_mem_to_from_reg.disp;
            break;
        case immediate_to_register_mem:
            return inst1.imm_to_reg_mem.disp;
            break;
        case register_mem_to_from_seg_register:
            return inst1.reg_mem_to_from_seg_reg.disp;
            break;
        }
        break;
    case RM::bx_plus_si_plus8:
    case RM::bx_plus_si_plus16:
        operand1 = cpu.regSlots[1];
        operand2 = cpu.regSlots[6];
        switch (inst1.op_tag)
        {
        case register_mem_to_from_register:
            operand3 = inst1.reg_mem_to_from_reg.disp;
            break;
        case immediate_to_register_mem:
            operand3 = inst1.imm_to_reg_mem.disp;
            break;
        case register_mem_to_from_seg_register:
            operand3 = inst1.reg_mem_to_from_seg_reg.disp;
            break;
        }
        return operand1 + operand2 + operand3;
        break;
    case RM::bx_plus_di_plus8:
    case RM::bx_plus_di_plus16:
        operand1 = cpu.regSlots[1];
        operand2 = cpu.regSlots[7];
        switch (inst1.op_tag)
        {
        case register_mem_to_from_register:
            operand3 = inst1.reg_mem_to_from_reg.disp;
            break;
        case immediate_to_register_mem:
            operand3 = inst1.imm_to_reg_mem.disp;
            break;
        case register_mem_to_from_seg_register:
            operand3 = inst1.reg_mem_to_from_seg_reg.disp;
            break;
        }
        return operand1 + operand2 + operand3;
        break;
    case RM::bp_plus_si_plus8:
    case RM::bp_plus_si_plus16:
        operand1 = cpu.regSlots[5];
        operand2 = cpu.regSlots[6];
        switch (inst1.op_tag)
        {
        case register_mem_to_from_register:
            operand3 = inst1.reg_mem_to_from_reg.disp;
            break;
        case immediate_to_register_mem:
            operand3 = inst1.imm_to_reg_mem.disp;
            break;
        case register_mem_to_from_seg_register:
            operand3 = inst1.reg_mem_to_from_seg_reg.disp;
            break;
        }
        return operand1 + operand2 + operand3;
        break;
    case RM::bp_plus_di_plus8:
    case RM::bp_plus_di_plus16:
        operand1 = cpu.regSlots[5];
        operand2 = cpu.regSlots[7];
        switch (inst1.op_tag)
        {
        case register_mem_to_from_register:
            operand3 = inst1.reg_mem_to_from_reg.disp;
            break;
        case immediate_to_register_mem:
            operand3 = inst1.imm_to_reg_mem.disp;
            break;
        case register_mem_to_from_seg_register:
            operand3 = inst1.reg_mem_to_from_seg_reg.disp;
            break;
        }
        return operand1 + operand2 + operand3;
        break;
    case RM::si_plus8:
    case RM::si_plus16:
        operand1 = cpu.regSlots[6];
        switch (inst1.op_tag)
        {
        case register_mem_to_from_register:
            operand2 = inst1.reg_mem_to_from_reg.disp;
            break;
        case immediate_to_register_mem:
            operand2 = inst1.imm_to_reg_mem.disp;
            break;
        case register_mem_to_from_seg_register:
            operand2 = inst1.reg_mem_to_from_seg_reg.disp;
            break;
        }
        return operand1 + operand2;
        break;
    case RM::di_plus8:
    case RM::di_plus16:
        operand1 = cpu.regSlots[7];
        switch (inst1.op_tag)
        {
        case register_mem_to_from_register:
            operand2 = inst1.reg_mem_to_from_reg.disp;
            break;
        case immediate_to_register_mem:
            operand2 = inst1.imm_to_reg_mem.disp;
            break;
        case register_mem_to_from_seg_register:
            operand2 = inst1.reg_mem_to_from_seg_reg.disp;
            break;
        }
        return operand1 + operand2;
        break;
    case RM::bp_plus8:
    case RM::bp_plus16:
        operand1 = cpu.regSlots[5];
        switch (inst1.op_tag)
        {
        case register_mem_to_from_register:
            operand2 = inst1.reg_mem_to_from_reg.disp;
            break;
        case immediate_to_register_mem:
            operand2 = inst1.imm_to_reg_mem.disp;
            break;
        case register_mem_to_from_seg_register:
            operand2 = inst1.reg_mem_to_from_seg_reg.disp;
            break;
        }
        return operand1 + operand2;
        break;
    case RM::bx_plus8:
    case RM::bx_plus16:
        operand1 = cpu.regSlots[1];
        switch (inst1.op_tag)
        {
        case register_mem_to_from_register:
            operand2 = inst1.reg_mem_to_from_reg.disp;
            break;
        case immediate_to_register_mem:
            operand2 = inst1.imm_to_reg_mem.disp;
            break;
        case register_mem_to_from_seg_register:
            operand2 = inst1.reg_mem_to_from_seg_reg.disp;
            break;
        }
        return operand1 + operand2;
        break;
    }
    cout << "Invalid register/memory location." << endl;
    exit(1);
}

int getCPUSlotSR(SR es)
{
    switch (es)
    {
    case SR::es:
        return 8;
        break;
    case SR::cs:
        return 9;
        break;
    case SR::ss:
        return 10;
        break;
    case SR::ds:
        return 11;
        break;
    default:
        return -1;
        break;
    }
}

void setFlags(instruction inst1, i32 result, i32 source, Flags &flag)
{
    // Parity Flag
    int parity = (result & 1);
    for (int i = 1; i < 9; i++)
    {
        parity += (((result & lowBitsMask) >> i) & singBitConv);
    }

    if (parity % 2 == 0)
    {
        flag.flags[13] = true;
    }
    else
    {
        flag.flags[13] = false;
    }

    // Zero Flag
    if (result == 0)
    {
        flag.flags[9] = true;
    }
    else
    {
        flag.flags[9] = false;
    }

    // Sign Flag
    if (inst1.w == Word)
    {
        if (((result >> 15) & 1) == 1)
        {
            flag.flags[8] = true;
        }
        else
        {
            flag.flags[8] = false;
        }
    }
    else
    {
        if (((result >> 7) & 1) == 1)
        {
            flag.flags[8] = true;
        }
        else
        {
            flag.flags[8] = false;
        }
    }

    // Carry Flag
    switch (inst1.mnemonic)
    {
    case add:
        if (result > 255)
        {
            flag.flags[15] = true;
        }
        else
        {
            flag.flags[15] = false;
        }
        break;
    case sub:
    case cmp:
        if (result < 0)
        {
            flag.flags[15] = true;
        }
        else
        {
            flag.flags[15] = false;
        }
        break;
    default:
        flag.flags[15] = false;
    }

    // Overflow Flag
    switch (inst1.mnemonic)
    {
    case add:
    case sub:
    case cmp:
        if (inst1.w == Word)
        {
            if ((result > 65535) || (result < -32768))
            {
                flag.flags[4] = true;
            }
            else
            {
                flag.flags[4] = false;
            }
        }
        else
        {
            if ((result > 255) || (result < -127))
            {
                flag.flags[4] = true;
            }
            else
            {
                flag.flags[4] = false;
            }
        }
        break;
    default:
        flag.flags[4] = false;
        break;
    }

    // Auxilliary Carry Flag
    i8 lowNibbleSource = (source & fourBitConv);
    i8 lowNibbleDestination = 0;
    switch (inst1.mnemonic)
    {
    case add:
        lowNibbleDestination = ((result - source) & fourBitConv);
        if ((lowNibbleDestination + lowNibbleSource) > 15)
        {
            flag.flags[11] = true;
        }
        else
        {
            flag.flags[11] = false;
        }
        break;
    case sub:
        lowNibbleDestination = ((result + source) & fourBitConv);
    case cmp:
        lowNibbleDestination = ((result + source) & fourBitConv);
        if (lowNibbleDestination >= lowNibbleSource)
        {
            flag.flags[11] = false;
        }
        else
        {
            flag.flags[11] = true;
        }
    }
}

void printFlags(Flags &flag)
{
    for (int i = 0; i < 16; i++)
    {
        if (flag.flags[i] != 0)
        {
            cout << " " << flagsList[i] << " ";
        }
    }
    cout << endl;
}