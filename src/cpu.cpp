#include "cpu.hpp"

#include <cassert>
#include <cstring>

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
    {"BRK", &Cpu::BRK, Implied, 7},     {"ORA", &Cpu::ORA, IndexedIndirect, 6},
    {"XXX", &Cpu::XXX, Implied, 0},     {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},     {"ORA", &Cpu::ORA, ZeroPage, 3},
    {"ASL", &Cpu::ASL, ZeroPage, 5},    {"XXX", &Cpu::XXX, Implied, 0},
    {"PHP", &Cpu::PHP, Implied, 3},     {"ORA", &Cpu::ORA, Immediate, 2},
    {"ASL", &Cpu::ASL, Accumulator, 2}, {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},     {"ORA", &Cpu::ORA, Absolute, 4},
    {"ASL", &Cpu::ASL, Absolute, 2},    {"XXX", &Cpu::XXX, Implied, 0},
    // 10 - 1F
    {"BPL",&Cpu::BPL,Relative,2},{"ORA", &Cpu::ORA, IndirectIndexed, 5},
    
    
};

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
    _PC += instruction_lookup_table[_opcode].size;
    _cycles += instruction_lookup_table[_opcode].cycle_lenght;
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
        if (is_page_crossed(temp, temp + _X))
            ++_cycles;
        return temp + _X;
    case AbsoluteY:
        temp = _mmc.read16(_PC + 1);
        if (is_page_crossed(temp, temp + _Y))
            ++_cycles;
        return temp + _Y;
    case Indirect:
        return _mmc.read16Bug(_mmc.read16(_PC + 1));
    case IndexedIndirect:
        return _mmc.read16(_mmc.read(_PC + 1) + _X);
    case IndirectIndexed:
        temp = _mmc.read16(_mmc.read(_PC + 1));
        if (is_page_crossed(temp, temp + _Y))
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

    if (is_page_crossed(_PC, _PC + displacement))
        _cycles += 2;

    _PC += displacement;
    ++_cycles;
}
bool Cpu::is_page_crossed(uint16_t a, uint16_t b)
{
    return (a & 0xFF00) != (b & 0xFF00);
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
    _PC = pull16();
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
    // TODO: trow
}
