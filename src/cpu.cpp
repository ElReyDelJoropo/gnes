#include "cpu.hpp"

#include <cassert>
#include <cstring>
#include <iomanip>
using std::hex;
using std::setw;
#include <string>
using std::string;
#include <stdexcept>
using std::runtime_error;

using namespace gnes;

const uint16_t Cpu::instruction_sizes[13] = {
    2, // Zero page
    2, // Zero page X
    2, // Zero page Y
    3, // Absolute X
    3, // Absolute Y
    2, // Indexed Indirect
    2, // Indirect Indexed
    1, // Implied
    1, // Accumulator
    2, // Immediate
    3, // Absolute
    2, // Relative
    3  // Indirect
};
const struct Cpu::instruction Cpu::instruction_lookup_table[0xFF] = {
    // 00-0F
    {"BRK", &Cpu::BRK, Implied, 7},
    {"ORA", &Cpu::ORA, IndexedIndirect, 6},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"ORA", &Cpu::ORA, ZeroPage, 3},
    {"ASL", &Cpu::ASL, ZeroPage, 5},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"PHP", &Cpu::PHP, Implied, 3},
    {"ORA", &Cpu::ORA, Immediate, 2},
    {"ASL", &Cpu::ASL, Accumulator, 2},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"ORA", &Cpu::ORA, Absolute, 4},
    {"ASL", &Cpu::ASL, Absolute, 6},
    {"XXX", &Cpu::XXX, Implied, 0},
    // 10 - 1F
    {"BPL", &Cpu::BPL, Relative, 2},
    {"ORA", &Cpu::ORA, IndirectIndexed, 5},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"ORA", &Cpu::ORA, ZeroPageX, 4},
    {"ASL", &Cpu::ASL, ZeroPageX, 6},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"CLC", &Cpu::CLC, Implied,2},
    {"ORA", &Cpu::ORA, AbsoluteY, 4},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"ORA", &Cpu::ORA, AbsoluteX, 4},
    {"ASL", &Cpu::ASL, AbsoluteX, 7},
    {"XXX", &Cpu::XXX, Implied, 0},
    //20 - 2F
    {"JSR",&Cpu::JSR,Absolute,6},
    {"AND",&Cpu::AND,IndexedIndirect,6},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"BIT",&Cpu::BIT,ZeroPage,3},
    {"AND",&Cpu::AND,ZeroPage,3},
    {"ROL",&Cpu::ROL,ZeroPage,5},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"PLP", &Cpu::PLP, Implied, 4},
    {"AND",&Cpu::AND,Immediate,2},
    {"ROL",&Cpu::ROL,Accumulator,2},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"BIT",&Cpu::BIT,Absolute,4},
    {"AND",&Cpu::AND,Absolute,4},
    {"ROL",&Cpu::ROL,Absolute,6},
    {"XXX", &Cpu::XXX, Implied, 0},
    //30 - 3F
    {"BMI", &Cpu::BMI, Relative, 2},
    {"AND",&Cpu::AND,IndirectIndexed,5},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"AND",&Cpu::AND,ZeroPageX,4},
    {"ROL",&Cpu::ROL,ZeroPageX,6},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"SEC", &Cpu::SEC, Implied, 2},

};

Cpu::Cpu(Mmc &m, InterruptLine &i, LogModule &l): _mmc{m}, _interrupt_line{i}, _log_module{l} {
    powerUp();
    reset();
}

void Cpu::powerUp()
{
    _P.data = 0x34;
    _A = _X = _Y = 0;
    _SP = 0xFD;
}
void Cpu::reset()
{
    // TODO: silence APU
    _interrupt_line.setInterrupt(InterruptType::Reset);
}
void Cpu::step()
{
    uint16_t address;

    handleInterrupt();

    _opcode = _mmc.read(_PC);
    address =
        translateAddress(instruction_lookup_table[_opcode].addressing_mode);
    _PC += instruction_sizes[instruction_lookup_table[_opcode].addressing_mode];
    _cycles += instruction_lookup_table[_opcode].cycle_lenght;
    dumpCpuState(address);
    (this->*instruction_lookup_table[_opcode].func)(address);
}

uint16_t Cpu::translateAddress(AddressingMode mode)
{
    uint16_t temp;
    switch (mode) {
    // Zero page and relative addressing fetch the address in the same way
    case ZeroPage:
    case Relative:
        return _mmc.read(_PC + 1);
    case ZeroPageX:
        return _mmc.read(_PC + 1) + _X & 0xFF; // Wrapped around
    case ZeroPageY:
        return _mmc.read(_PC + 1) + _Y & 0xFF;
    case Absolute:
        return _mmc.read16(_PC + 1);
    case AbsoluteX:
        temp = _mmc.read16(_PC + 1);
        if (isPageCrossed(temp, temp + _X))
            ++_cycles;
        return temp + _X;
    case AbsoluteY:
        temp = _mmc.read16(_PC + 1);
        if (isPageCrossed(temp, temp + _Y))
            ++_cycles;
        return temp + _Y;
    case Indirect:
        return _mmc.read16Bug(_mmc.read16(_PC + 1));
    case IndexedIndirect:
        return _mmc.read16(_mmc.read(_PC + 1) + _X);
    case IndirectIndexed:
        temp = _mmc.read16(_mmc.read(_PC + 1));
        if (isPageCrossed(temp, temp + _Y))
            ++_cycles;
        return temp + _Y;
    case Immediate:
        return _PC + 1;
    case Implied:
    case Accumulator:
        return 0;
    }
}
void Cpu::handleInterrupt()
{
    uint16_t vector_address;
    InterruptType interrupt = _interrupt_line.getInterrupt();
    // Should i put reset as interrupt type?
    if (interrupt == InterruptType::None)
        return;

    if (interrupt == InterruptType::Irq) {
        if (_P.interrupt_disable)
            return;
        vector_address = IRQ_ADDRESS;
    } else if (interrupt == InterruptType::Nmi) {
        vector_address = NMI_ADDRESS;
    } else {
        // Nesdev wiki says that reset is like an interrupt, SP decrements by 3
        // and registers stay unchanged, therefore, i included here
        vector_address = RESET_ADDRESS;
    }

    _P.interrupt_disable = 1;
    push16(_PC);
    push(_P.data);
    _PC = _mmc.read16(vector_address);
    // XXX: this should be done here?
    _cycles += 7; // Cycles per interrupt handling
    _interrupt_line.clear();
}

void Cpu::push(uByte b)
{
    _mmc.write(_SP | 0x100, b);
    --_SP; // Stack grows downward
}
void Cpu::push16(uint16_t b)
{
    push(b >> 8);
    push(b);
}

uByte Cpu::pull() { return _mmc.read(++_SP | 0x100); }
uint16_t Cpu::pull16()
{
    uint16_t low = pull();
    uint16_t high = pull();

    return high << 8 | low;
}

void Cpu::branch(uint16_t address)
{
    // XXX: Check that cpu branchs correctly
    Byte displacement = _mmc.read(address); // Signed byte

    if (isPageCrossed(_PC, _PC + displacement))
        _cycles += 2;

    _PC += displacement;
    ++_cycles;
}
bool Cpu::isPageCrossed(uint16_t a, uint16_t b)
{
    return (a & 0xFF00) != (b & 0xFF00);
}

void Cpu::dumpCpuState(uint16_t address)
{
    _log_module.setBufferID(BufferID::CpuID);
    _log_module << "-> "
                << assembleInstruction(instruction_lookup_table[_opcode].name,
                                       address)
                << '\n'
                << "PC: $" << setw(4) << hex << (int)_PC << '\n'
                << "X: $" << setw(2) << (int)_X << '\t' << "Y: $" << setw(2)
                << (int)_Y << '\n'
                << "SP: $" << setw(2) << (int)_SP << '\n'
                << "P: " << statusRegisterToString() << '\n';
}
string Cpu::assembleInstruction(string name, uint16_t address)
{
    auto toHexString = [](int value, int wide) {
        assert(value >= 0 && value <= 0xFFFF);
        assert(wide == 4 || (wide == 2 && (value & 0xFF00) == 0));

        char buf[5]; // 16 bits address are 4digits wide
        snprintf(buf, 5, "%0*X", wide, value);

        return "$" + string{buf};
    };

    switch (instruction_lookup_table[_opcode].addressing_mode) {
    case ZeroPage:
        return name + " " + toHexString(address, 2);
    case ZeroPageX:
        return name + " " + toHexString(address, 2) + ", X";
    case ZeroPageY:
        return name + " " + toHexString(address, 2) + ", Y";
    case Absolute:
        return name + " " + toHexString(address, 4);
    case AbsoluteX:
        return name + " " + toHexString(address, 4) + ", X";
    case AbsoluteY:
        return name + " " + toHexString(address, 4) + ", Y";
    case Indirect:
        return name + " (" + toHexString(address, 4) + ")";
    case IndexedIndirect:
        return name + " (" + toHexString(address, 2) + ", X)";
    case IndirectIndexed:
        return name + " (" + toHexString(address, 4) + "), Y";
    case Immediate:
        return name + " #" + toHexString(address, 2);
    case Relative:
        return name + toHexString(static_cast<Byte>(address), 2);
    case Implied:
    case Accumulator:
        return "";
    }
}

void Cpu::ADC(uint16_t address)
{
    uByte temp = _mmc.read(address);
    uint16_t result = _A + temp + _P.carry;

    _P.zero = ~_A;
    _P.negative = _A & 0x80;
    _P.carry = result & 0x100; // The easy way
    _P.overflow = (_A ^ result) & (temp ^ result) & 0x80;

    _A = result; // result is truncated
}
void Cpu::AND(uint16_t address)
{
    _A &= _mmc.read(address);
    _P.zero = ~_A;
    _P.negative = _A & 0x80;
}
void Cpu::ASL(std::uint16_t address)
{
    if (instruction_lookup_table[_opcode].addressing_mode == Accumulator) {
        _P.carry = _A & 0x80;
        _A <<= 1;
        _P.zero = ~_A;
        _P.negative = _A & 0x80;
    } else {
        uByte temp = _mmc.read(address);

        _P.carry = temp & 0x80;
        temp <<= 1;
        _P.zero = ~temp;
        _P.negative = temp & 0x80;
        _mmc.write(address, temp);
    }
}
void Cpu::BCC(uint16_t address)
{
    if (!_P.carry)
        branch(address);
}
void Cpu::BCS(uint16_t address)
{
    if (_P.carry)
        branch(address);
}
void Cpu::BEQ(uint16_t address)
{
    if (_P.zero)
        branch(address);
}
void Cpu::BIT(uint16_t address)
{
    uByte temp = _mmc.read(address);

    _P.overflow = temp & 0x40;
    _P.negative = temp & 0x70;
    _P.zero = _A & temp;
}
void Cpu::BMI(uint16_t address)
{
    if (_P.negative)
        branch(address);
}
void Cpu::BNE(uint16_t address)
{
    if (!_P.zero)
        branch(address);
}
void Cpu::BPL(uint16_t address)
{
    if (!_P.negative)
        branch(address);
}
void Cpu::BRK(uint16_t)
{
    // XXX: Be careful with brk flag
    _P.brk = 1;
    _interrupt_line.setInterrupt(InterruptType::Irq);
}
void Cpu::BVC(uint16_t address)
{
    if (!_P.overflow)
        branch(address);
}
void Cpu::BVS(uint16_t address)
{
    if (_P.overflow)
        branch(address);
}
void Cpu::CLC(uint16_t) { _P.carry = 0; }
void Cpu::CLD(uint16_t) { _P.decimal = 0; }
void Cpu::CLI(uint16_t) { _P.interrupt_disable = 0; }
void Cpu::CLV(uint16_t) { _P.overflow = 0; }
void Cpu::CMP(uint16_t address)
{
    // XXX: This function is weird
    uByte temp = _mmc.read(address);

    _P.carry = _A >= temp;
    _P.zero = _A == temp;
    _A -= temp;
    _P.negative = _A & 0x80;
}
void Cpu::CPX(uint16_t address)
{
    // XXX: This function is weird
    uByte temp = _mmc.read(address);

    _P.carry = _X >= temp;
    _P.zero = _X == temp;
    _X -= temp;
    _P.negative = _X & 0x80;
}
void Cpu::CPY(uint16_t address)
{
    // XXX: This function is weird
    uByte temp = _mmc.read(address);

    _P.carry = _Y >= temp;
    _P.zero = _Y == temp;
    _Y -= temp;
    _P.negative = _Y & 0x80;
}
void Cpu::DEC(uint16_t address)
{
    uByte temp = _mmc.read(address);
    --temp;
    _P.zero = ~temp;
    _P.negative = temp & 0x80;
    _mmc.write(address, temp);
}
void Cpu::DEX(uint16_t address)
{
    --_X;
    _P.zero = ~_X;
    _P.negative = _X & 0x80;
    _mmc.write(address, _X);
}
void Cpu::DEY(uint16_t address)
{
    --_Y;
    _P.zero = ~_Y;
    _P.negative = _Y & 0x80;
    _mmc.write(address, _Y);
}
void Cpu::EOR(uint16_t address)
{
    _A ^= _mmc.read(address);
    _P.zero = ~_A;
    _P.negative = _A & 0x80;
}
void Cpu::INC(uint16_t address)
{
    uByte temp = _mmc.read(address);
    ++temp;
    _P.zero = ~temp;
    _P.negative = temp & 0x80;
    _mmc.write(address, temp);
}
void Cpu::INX(uint16_t address)
{
    ++_X;
    _P.zero = ~_X;
    _P.negative = _X & 0x80;
    _mmc.write(address, _X);
}
void Cpu::INY(uint16_t address)
{
    ++_Y;
    _P.zero = ~_Y;
    _P.negative = _Y & 0x80;
    _mmc.write(address, _Y);
}
void Cpu::JMP(uint16_t address)
{
    // TODO: treat this function as size 0
    // XXX: Check this function and add the 6502 bug
    _PC = address;
}
void Cpu::JSR(uint16_t address)
{
    // TODO: this is a size 0 instruction
    push16(_PC - 1);
    _PC = address;
}
void Cpu::LDA(uint16_t address)
{
    _A = _mmc.read(address);
    _P.zero = ~_A;
    _P.negative = _A & 0x80;
}
void Cpu::LDX(uint16_t address)
{
    _X = _mmc.read(address);
    _P.zero = ~_X;
    _P.negative = _X & 0x80;
}
void Cpu::LDY(uint16_t address)
{
    _Y = _mmc.read(address);
    _P.zero = ~_Y;
    _P.negative = _Y & 0x80;
}
void Cpu::LSR(uint16_t address)
{
    if (instruction_lookup_table[_opcode].addressing_mode == Accumulator) {
        _P.carry = _A & 0x1;
        _A >>= 1;
        _P.zero = ~_A;
        _P.negative = _A & 0x80;
    } else {
        uByte temp = _mmc.read(address);
        _P.carry = temp & 0x1;
        temp >>= 1;
        _P.zero = ~temp;
        _P.negative = temp & 0x80;
        _mmc.write(address, temp);
    }
}
void Cpu::NOP(uint16_t) {}
void Cpu::ORA(uint16_t address)
{
    _A |= _mmc.read(address);
    _P.zero = ~_A;
    _P.negative = _A & 0x80;
}
void Cpu::PHA(uint16_t) { push(_A); }
void Cpu::PHP(uint16_t) { push(_P.data); }
void Cpu::PLA(uint16_t)
{
    _A = pull();
    _P.zero = ~_A;
    _P.negative = _A & 0x80;
}
void Cpu::PLP(uint16_t) { _P.data = pull(); }
void Cpu::ROL(uint16_t address)
{
    uByte old_carry = _P.carry;

    assert(old_carry == 1 || old_carry == 0);

    if (instruction_lookup_table[_opcode].addressing_mode == Accumulator) {
        _P.carry = _A & 0x80;
        _A = _A << 1 | old_carry;
        _P.zero = ~_A;
        _P.negative = _A & 0x80;
    } else {
        uByte temp = _mmc.read(address);
        _P.carry = temp & 0x80;
        temp = temp << 1 | old_carry;
        _P.zero = ~temp;
        _P.negative = temp & 0x80;
    }
}
void Cpu::ROR(uint16_t address)
{
    uByte old_carry = _P.carry << 7;

    assert(old_carry == 0x80 || old_carry == 0);

    if (instruction_lookup_table[_opcode].addressing_mode == Accumulator) {
        _P.carry = _A & 0x1;
        _A = _A >> 1 | old_carry;
        _P.zero = ~_A;
        _P.negative = _A & 0x80;
    } else {
        uByte temp = _mmc.read(address);
        _P.carry = temp & 0x1;
        temp = temp >> 1 | old_carry;
        _P.zero = ~temp;
        _P.negative = temp & 0x80;
    }
}
void Cpu::RTI(uint16_t)
{
    _P.data = pull();
    _PC = pull16();
}
void Cpu::RTS(uint16_t)
{
    // TODO: check this instruction size
    _PC = pull16() + 1;
    assert(instruction_lookup_table[_PC - 3].name == "JSR");
}
void Cpu::SBC(uint16_t address)
{
    // XXX: take care about this function
    uByte b = _mmc.read(address);
    uint16_t result = _A - b - ~_P.carry;

    _P.zero = ~result;
    _P.negative = result & 0x80;
    _P.carry = result & 0x100;
    _P.overflow = (_A ^ result) & (b ^ result) & 0x80;
    _A = result;
}
void Cpu::SEC(uint16_t) { _P.carry = 1; }
void Cpu::SED(uint16_t) { _P.decimal = 1; }
void Cpu::SEI(uint16_t) { _P.interrupt_disable = 1; }
void Cpu::STA(uint16_t address) { _mmc.write(address, _A); }
void Cpu::STX(uint16_t address) { _mmc.write(address, _X); }
void Cpu::STY(uint16_t address) { _mmc.write(address, _Y); }
void Cpu::TAX(uint16_t)
{
    _X = _A;
    _P.zero = ~_X;
    _P.negative = _X & 0x80;
}
void Cpu::TAY(uint16_t)
{
    _Y = _A;
    _P.zero = ~_Y;
    _P.negative = _Y & 0x80;
}
void Cpu::TSX(uint16_t)
{
    _X = _SP;
    _P.zero = ~_X;
    _P.negative = _X & 0x80;
}
void Cpu::TXA(uint16_t)
{
    _A = _X;
    _P.zero = ~_A;
    _P.negative = _A & 0x80;
}
void Cpu::TXS(uint16_t) { _SP = _X; }
void Cpu::TYA(uint16_t)
{
    _A = _Y;
    _P.zero = ~_A;
    _P.negative = _A & 0x80;
}
void Cpu::XXX(uint16_t)
{
    throw runtime_error{"Cpu: Invalid opcode"};
}
