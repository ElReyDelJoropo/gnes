#include "Cpu.hpp"

#include <cassert>
#include <cstring>
#include <iomanip>
using std::hex;
using std::setfill;
using std::setw;
#include <sstream>
using std::ostringstream;
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
const struct Cpu::instruction Cpu::instruction_lookup_table[0x100] = {
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
    {"CLC", &Cpu::CLC, Implied, 2},
    {"ORA", &Cpu::ORA, AbsoluteY, 4},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"ORA", &Cpu::ORA, AbsoluteX, 4},
    {"ASL", &Cpu::ASL, AbsoluteX, 7},
    {"XXX", &Cpu::XXX, Implied, 0},
    // 20 - 2F
    {"JSR", &Cpu::JSR, Absolute, 6},
    {"AND", &Cpu::AND, IndexedIndirect, 6},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"BIT", &Cpu::BIT, ZeroPage, 3},
    {"AND", &Cpu::AND, ZeroPage, 3},
    {"ROL", &Cpu::ROL, ZeroPage, 5},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"PLP", &Cpu::PLP, Implied, 4},
    {"AND", &Cpu::AND, Immediate, 2},
    {"ROL", &Cpu::ROL, Accumulator, 2},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"BIT", &Cpu::BIT, Absolute, 4},
    {"AND", &Cpu::AND, Absolute, 4},
    {"ROL", &Cpu::ROL, Absolute, 6},
    {"XXX", &Cpu::XXX, Implied, 0},
    // 30 - 3F
    {"BMI", &Cpu::BMI, Relative, 2},
    {"AND", &Cpu::AND, IndirectIndexed, 5},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"AND", &Cpu::AND, ZeroPageX, 4},
    {"ROL", &Cpu::ROL, ZeroPageX, 6},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"SEC", &Cpu::SEC, Implied, 2},
    {"AND", &Cpu::AND, AbsoluteY, 4},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"AND", &Cpu::AND, AbsoluteX, 4},
    {"ROL", &Cpu::ROL, AbsoluteX, 7},
    {"XXX", &Cpu::XXX, Implied, 0},
    // 40 - 4F
    {"RTI", &Cpu::RTI, Implied, 6},
    {"EOR", &Cpu::EOR, IndexedIndirect, 6},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"EOR", &Cpu::EOR, ZeroPage, 3},
    {"LSR", &Cpu::LSR, ZeroPage, 5},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"PHA", &Cpu::PHA, Implied, 3},
    {"EOR", &Cpu::EOR, Immediate, 2},
    {"LSR", &Cpu::LSR, Accumulator, 2},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"JMP", &Cpu::JMP, Absolute, 3},
    {"EOR", &Cpu::EOR, Absolute, 4},
    {"LSR", &Cpu::LSR, Absolute, 6},
    {"XXX", &Cpu::XXX, Implied, 0},
    // 50 - 5F
    {"BVC", &Cpu::BVC, Relative, 2},
    {"EOR", &Cpu::EOR, IndirectIndexed, 5},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"EOR", &Cpu::EOR, ZeroPageX, 4},
    {"LSR", &Cpu::LSR, ZeroPageX, 6},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"CLI", &Cpu::CLI, Implied, 2},
    {"EOR", &Cpu::EOR, AbsoluteY, 4},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"EOR", &Cpu::EOR, AbsoluteX, 4},
    {"LSR", &Cpu::LSR, AbsoluteX, 7},
    {"XXX", &Cpu::XXX, Implied, 0},
    // 60 - 6F
    {"RTS", &Cpu::RTS, Implied, 6},
    {"ADC", &Cpu::ADC, IndexedIndirect, 6},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"ADC", &Cpu::ADC, ZeroPage, 3},
    {"ROR", &Cpu::ROR, ZeroPage, 5},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"PLA", &Cpu::PLA, Implied, 4},
    {"ADC", &Cpu::ADC, Immediate, 2},
    {"ROR", &Cpu::ROR, Accumulator, 2},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"JMP", &Cpu::JMP, Indirect, 5},
    {"ADC", &Cpu::ADC, Absolute, 4},
    {"ROR", &Cpu::ROR, Absolute, 6},
    {"XXX", &Cpu::XXX, Implied, 0},
    // 70 - 7F
    {"BVS", &Cpu::BVS, Relative, 2},
    {"ADC", &Cpu::ADC, IndirectIndexed, 5},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"ADC", &Cpu::ADC, ZeroPageX, 4},
    {"ROR", &Cpu::ROR, ZeroPageX, 6},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"SEI", &Cpu::SEI, Implied, 2},
    {"ADC", &Cpu::ADC, AbsoluteY, 4},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"ADC", &Cpu::ADC, AbsoluteX, 4},
    {"ROR", &Cpu::ROR, AbsoluteX, 7},
    {"XXX", &Cpu::XXX, Implied, 0},
    // 80 - 8F
    {"XXX", &Cpu::XXX, Implied, 0},
    {"STA", &Cpu::STA, IndexedIndirect, 6},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"STY", &Cpu::STY, ZeroPage, 3},
    {"STA", &Cpu::STA, ZeroPage, 3},
    {"STX", &Cpu::STX, ZeroPage, 3},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"DEY", &Cpu::DEY, Implied, 2},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"TXA", &Cpu::TXA, Implied, 2},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"STY", &Cpu::STY, Absolute, 4},
    {"STA", &Cpu::STA, Absolute, 4},
    {"STX", &Cpu::STX, Absolute, 4},
    {"XXX", &Cpu::XXX, Implied, 0},
    // 90- 9F
    {"BCC", &Cpu::BCC, Relative, 2},
    {"STA", &Cpu::STA, IndirectIndexed, 6},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"STY", &Cpu::STY, ZeroPageX, 4},
    {"STA", &Cpu::STA, ZeroPageX, 4},
    {"STX", &Cpu::STX, ZeroPageY, 4},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"TYA", &Cpu::TYA, Implied, 2},
    {"STA", &Cpu::STA, AbsoluteY, 5},
    {"TXS", &Cpu::TXS, Implied, 2},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"STA", &Cpu::STA, AbsoluteX, 5},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    // A0 - AF
    {"LDY", &Cpu::LDY, Immediate, 2},
    {"LDA", &Cpu::LDA, IndexedIndirect, 6},
    {"LDX", &Cpu::LDX, Immediate, 2},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"LDY", &Cpu::LDY, ZeroPage, 3},
    {"LDA", &Cpu::LDA, ZeroPage, 3},
    {"LDX", &Cpu::LDX, ZeroPage, 3},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"TAY", &Cpu::TAY, Implied, 2},
    {"LDA", &Cpu::LDA, Immediate, 2},
    {"TAX", &Cpu::TAX, Implied, 2},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"LDY", &Cpu::LDY, Absolute, 4},
    {"LDA", &Cpu::LDA, Absolute, 4},
    {"LDX", &Cpu::LDX, Absolute, 4},
    {"XXX", &Cpu::XXX, Implied, 0},
    // B0 - BF
    {"BCS", &Cpu::BCS, Relative, 2},
    {"LDA", &Cpu::LDA, IndirectIndexed, 5},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"LDY", &Cpu::LDY, ZeroPageX, 4},
    {"LDA", &Cpu::LDA, ZeroPageX, 4},
    {"LDX", &Cpu::LDX, ZeroPageY, 4},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"CLV", &Cpu::CLV, Implied, 2},
    {"LDA", &Cpu::LDA, AbsoluteY, 4},
    {"TSX", &Cpu::TSX, Implied, 2},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"LDY", &Cpu::LDY, AbsoluteX, 4},
    {"LDA", &Cpu::LDA, AbsoluteX, 4},
    {"LDX", &Cpu::LDX, AbsoluteY, 4},
    {"XXX", &Cpu::XXX, Implied, 0},
    // C0 - CF
    {"CPY", &Cpu::CPY, Immediate, 2},
    {"CMP", &Cpu::CMP, IndexedIndirect, 6},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"CPY", &Cpu::CPY, ZeroPage, 3},
    {"CMP", &Cpu::CMP, ZeroPage, 3},
    {"DEC", &Cpu::DEC, ZeroPage, 5},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"INY", &Cpu::INY, Implied, 2},
    {"CMP", &Cpu::CMP, Immediate, 2},
    {"DEX", &Cpu::DEX, Implied, 2},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"CPY", &Cpu::CPY, Absolute, 4},
    {"CMP", &Cpu::CMP, Absolute, 4},
    {"DEC", &Cpu::DEC, Absolute, 6},
    {"XXX", &Cpu::XXX, Implied, 0},
    // D0 - DF
    {"BNE", &Cpu::BNE, Relative, 2},
    {"CMP", &Cpu::CMP, IndirectIndexed, 5},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"CMP", &Cpu::CMP, ZeroPageX, 4},
    {"DEC", &Cpu::DEC, ZeroPageX, 6},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"CLD", &Cpu::CLD, Implied, 2},
    {"CMP", &Cpu::CMP, AbsoluteY, 4},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"CMP", &Cpu::CMP, AbsoluteX, 4},
    {"DEC", &Cpu::DEC, AbsoluteX, 7},
    {"XXX", &Cpu::XXX, Implied, 0},
    // E0 - EF
    {"CPX", &Cpu::CPX, Immediate, 2},
    {"SBC", &Cpu::SBC, IndexedIndirect, 6},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"CPX", &Cpu::CPX, ZeroPage, 3},
    {"SBC", &Cpu::SBC, ZeroPage, 3},
    {"INC", &Cpu::INC, ZeroPage, 5},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"INX", &Cpu::INX, Implied, 2},
    {"SBC", &Cpu::SBC, Immediate, 2},
    {"NOP", &Cpu::NOP, Implied, 2},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"CPX", &Cpu::CPX, Absolute, 4},
    {"SBC", &Cpu::SBC, Absolute, 4},
    {"INC", &Cpu::INC, Absolute, 6},
    {"XXX", &Cpu::XXX, Implied, 0},
    // F0 - FF
    {"BEQ", &Cpu::BEQ, Relative, 2},
    {"SBC", &Cpu::SBC, IndirectIndexed, 5},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"SBC", &Cpu::SBC, ZeroPageX, 4},
    {"INC", &Cpu::INC, ZeroPageX, 6},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"SED", &Cpu::SED, Implied, 2},
    {"SBC", &Cpu::SBC, AbsoluteY, 4},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"XXX", &Cpu::XXX, Implied, 0},
    {"SBC", &Cpu::SBC, AbsoluteX, 4},
    {"INC", &Cpu::INC, AbsoluteX, 7},
    {"XXX", &Cpu::XXX, Implied, 0},
};

Cpu::Cpu(CpuBus *b, InterruptLine *i, LogModule *l)
    : _bus(b), _interrupt_line(i), _log_module(l)
{
    powerUp();
    reset();
}

void Cpu::powerUp()
{
    _p.data = 0x34;
    _a = _x = _y = 0;
    _sp = 0xFD;
}
void Cpu::reset()
{
    // TODO: silence APU
    _interrupt_line->setInterrupt(InterruptType::Reset);
}
void Cpu::step()
{
    handleInterrupt();

    _opcode = _bus->read(_pc);
    uint16_t address =
        translateAddress(instruction_lookup_table[_opcode].addressing_mode);
    dumpCpuState(address);
    _pc += instruction_sizes[instruction_lookup_table[_opcode].addressing_mode];
    _cycles += instruction_lookup_table[_opcode].cycle_lenght;
    (this->*instruction_lookup_table[_opcode].func)(address);
}

uint16_t Cpu::translateAddress(AddressingMode mode)
{
    uint16_t temp;
    switch (mode) {
    case ZeroPage:
        return _bus->read(_pc + 1);
    case ZeroPageX:
        return _bus->read(_pc + 1) + _x & 0xFF; // Wrapped around
    case ZeroPageY:
        return _bus->read(_pc + 1) + _y & 0xFF;
    case Absolute:
        return _bus->read16(_pc + 1);
    case AbsoluteX:
        temp = _bus->read16(_pc + 1);
        if (isPageCrossed(temp, temp + _x))
            ++_cycles;
        return temp + _x;
    case AbsoluteY:
        temp = _bus->read16(_pc + 1);
        if (isPageCrossed(temp, temp + _y))
            ++_cycles;
        return temp + _y;
    case Indirect:
        return _bus->read16Bug(_bus->read16(_pc + 1));
    case IndexedIndirect:
        return _bus->read16(_bus->read(_pc + 1) + _x);
    case IndirectIndexed:
        temp = _bus->read16(_bus->read(_pc + 1));
        if (isPageCrossed(temp, temp + _y))
            ++_cycles;
        return temp + _y;
    case Immediate:
    case Relative:
        return _pc + 1;
    case Implied:
    case Accumulator:
        return 0;
    }
}
void Cpu::handleInterrupt()
{
    uint16_t vector_address;
    InterruptType interrupt = _interrupt_line->getInterrupt();
    // Should i put reset as interrupt type?
    if (interrupt == InterruptType::None)
        return;

    if (interrupt == InterruptType::Irq) {
        if (_p.interrupt_disable)
            return;
        vector_address = IRQ_ADDRESS;
    } else if (interrupt == InterruptType::Nmi) {
        vector_address = NMI_ADDRESS;
    } else {
        // Nesdev wiki says that reset is like an interrupt, SP decrements by 3
        // and registers stay unchanged, therefore, i included here
        vector_address = RESET_ADDRESS;
    }

    _p.interrupt_disable = 1;
    push16(_pc);
    push(_p.data);
    _pc = _bus->read16(vector_address);
    // XXX: this should be done here?
    _cycles += 7; // Cycles per interrupt handling
    _interrupt_line->clear();
}

void Cpu::push(uByte b)
{
    _bus->write(_sp | 0x100, b);
    --_sp; // Stack grows downward
}
void Cpu::push16(uint16_t b)
{
    push(b >> 8);
    push(b);
}

uByte Cpu::pull() { return _bus->read(++_sp | 0x100); }
uint16_t Cpu::pull16()
{
    uint16_t low = pull();
    uint16_t high = pull();

    return high << 8 | low;
}

void Cpu::branch(uint16_t address)
{
    // XXX: Check that cpu branchs correctly
    Byte displacement = static_cast<Byte>(_bus->read(address)); // Signed byte

    if (isPageCrossed(_pc, _pc + displacement))
        _cycles += 2;

    _pc += displacement;
    ++_cycles;
}
bool Cpu::isPageCrossed(uint16_t a, uint16_t b)
{
    return (a & 0xFF00) != (b & 0xFF00);
}

void Cpu::dumpCpuState(uint16_t address)
{
    _log_module->getBuffer(BufferID::CpuID)
        << assembleInstruction(instruction_lookup_table[_opcode].name, address)
        << '\t' << setfill('0') << "PC: $" << setw(4) << hex << (int)_pc << '\t'
        << "A: $" << setw(2) << (int)_a << '\t' << "X: $" << setw(2) << (int)_x
        << '\t' << "Y: $" << setw(2) << (int)_y << '\t' << "SP: $" << setw(2)
        << (int)_sp << '\t' << "P: " << statusRegisterToString() << '\n';
}
string Cpu::assembleInstruction(const char *name, uint16_t address) const
{
    ostringstream os;
    os.setf(std::ios::hex | std::ios::left);

    switch (instruction_lookup_table[_opcode].addressing_mode) {
    case ZeroPage:
        os << name << " $" << setw(4) << hex << address;
        break;
    case ZeroPageX:
        os << name << " $" << setw(4) << hex << address << ", X";
        break;
    case ZeroPageY:
        os << name << " $" << setw(4) << hex << address << ", Y";
        break;
    case Absolute:
        os << name << " $" << setw(4) << hex << address;
        break;
    case AbsoluteX:
        os << name << " $" << setw(4) << hex << address << ", X";
        break;
    case AbsoluteY:
        os << name << " $" << setw(4) << hex << address << ", Y";
        break;
    case Indirect:
        os << name << " ("
           << "$" << setw(4) << hex << address << ")";
        break;
    case IndexedIndirect:
        os << name << " ("
           << "$" << setw(4) << hex << address << ", X)";
        break;
    case IndirectIndexed:
        os << name << " ("
           << "$" << setw(4) << hex << address << "), Y)";
        break;
    case Immediate:
        os << name << " #"
           << "$" << setw(3) << hex << (int)_bus->read(address);
        break;
    case Relative:
        os << name << " " << std::showpos << setw(4)
           << static_cast<int>(static_cast<Byte>(address));
        break;
    case Implied:
    case Accumulator:
        os << name << setw(6) << ' ';
        break;
    }
    return os.str();
}
string Cpu::statusRegisterToString() const
{
    string ret{};
    ret += _p.carry ? "C" : "*";
    ret += _p.zero ? "Z" : "*";
    ret += _p.interrupt_disable ? "I" : "*";
    ret += _p.decimal ? "D" : "*";
    ret += _p.brk ? "B" : "*";
    ret += _p.unused ? "U" : "*";
    ret += _p.overflow ? "V" : "*";
    ret += _p.negative ? "N" : "*";

    return ret;
}

void Cpu::ADC(uint16_t address)
{
    uByte temp = _bus->read(address);
    uint16_t result = _a + temp + _p.carry;

    _p.zero = !_a;
    _p.negative = _a & 0x80;
    _p.carry = result & 0x100; // The easy way
    _p.overflow = (_a ^ result) & (temp ^ result) & 0x80;

    _a = result; // result is truncated
}
void Cpu::AND(uint16_t address)
{
    _a &= _bus->read(address);
    _p.zero = !_a;
    _p.negative = _a & 0x80;
}
void Cpu::ASL(std::uint16_t address)
{
    if (instruction_lookup_table[_opcode].addressing_mode == Accumulator) {
        _p.carry = _a & 0x80;
        _a <<= 1;
        _p.zero = !_a;
        _p.negative = _a & 0x80;
    } else {
        uByte temp = _bus->read(address);

        _p.carry = temp & 0x80;
        temp <<= 1;
        _p.zero = !temp;
        _p.negative = temp & 0x80;
        _bus->write(address, temp);
    }
}
void Cpu::BCC(uint16_t address)
{
    if (!_p.carry)
        branch(address);
}
void Cpu::BCS(uint16_t address)
{
    if (_p.carry)
        branch(address);
}
void Cpu::BEQ(uint16_t address)
{
    if (_p.zero)
        branch(address);
}
void Cpu::BIT(uint16_t address)
{
    uByte temp = _bus->read(address);

    _p.overflow = temp & 0x40;
    _p.negative = temp & 0x70;
    _p.zero = _a & temp;
}
void Cpu::BMI(uint16_t address)
{
    if (_p.negative)
        branch(address);
}
void Cpu::BNE(uint16_t address)
{
    if (!_p.zero)
        branch(address);
}
void Cpu::BPL(uint16_t address)
{
    if (!_p.negative)
        branch(address);
}
void Cpu::BRK(uint16_t)
{
    // XXX: Be careful with brk flag
    _p.brk = 1;
    _interrupt_line->setInterrupt(InterruptType::Irq);
}
void Cpu::BVC(uint16_t address)
{
    if (!_p.overflow)
        branch(address);
}
void Cpu::BVS(uint16_t address)
{
    if (_p.overflow)
        branch(address);
}
void Cpu::CLC(uint16_t) { _p.carry = 0; }
void Cpu::CLD(uint16_t) { _p.decimal = 0; }
void Cpu::CLI(uint16_t) { _p.interrupt_disable = 0; }
void Cpu::CLV(uint16_t) { _p.overflow = 0; }
void Cpu::CMP(uint16_t address)
{
    // XXX: This function is weird
    uByte temp = _bus->read(address);

    _p.carry = _a >= temp;
    _p.zero = _a == temp;
    _a -= temp;
    _p.negative = _a & 0x80;
}
void Cpu::CPX(uint16_t address)
{
    // XXX: This function is weird
    uByte temp = _bus->read(address);

    _p.carry = _x >= temp;
    _p.zero = _x == temp;
    _x -= temp;
    _p.negative = _x & 0x80;
}
void Cpu::CPY(uint16_t address)
{
    // XXX: This function is weird
    uByte temp = _bus->read(address);

    _p.carry = _y >= temp;
    _p.zero = _y == temp;
    _y -= temp;
    _p.negative = _y & 0x80;
}
void Cpu::DEC(uint16_t address)
{
    uByte temp = _bus->read(address);
    --temp;
    _p.zero = !temp;
    _p.negative = temp & 0x80;
    _bus->write(address, temp);
}
void Cpu::DEX(uint16_t address)
{
    --_x;
    _p.zero = !_x;
    _p.negative = _x & 0x80;
    _bus->write(address, _x);
}
void Cpu::DEY(uint16_t address)
{
    --_y;
    _p.zero = !_y;
    _p.negative = _y & 0x80;
    _bus->write(address, _y);
}
void Cpu::EOR(uint16_t address)
{
    _a ^= _bus->read(address);
    _p.zero = !_a;
    _p.negative = _a & 0x80;
}
void Cpu::INC(uint16_t address)
{
    uByte temp = _bus->read(address);
    ++temp;
    _p.zero = !temp;
    _p.negative = temp & 0x80;
    _bus->write(address, temp);
}
void Cpu::INX(uint16_t address)
{
    ++_x;
    _p.zero = !_x;
    _p.negative = _x & 0x80;
    _bus->write(address, _x);
}
void Cpu::INY(uint16_t address)
{
    ++_y;
    _p.zero = !_y;
    _p.negative = _y & 0x80;
    _bus->write(address, _y);
}
void Cpu::JMP(uint16_t address)
{
    // TODO: treat this function as size 0
    // XXX: Check this function and add the 6502 bug
    _pc = address;
}
void Cpu::JSR(uint16_t address)
{
    // TODO: this is a size 0 instruction
    push16(_pc - 1);
    _pc = address;
}
void Cpu::LDA(uint16_t address)
{
    _a = _bus->read(address);
    _p.zero = !_a;
    _p.negative = _a & 0x80;
}
void Cpu::LDX(uint16_t address)
{
    _x = _bus->read(address);
    _p.zero = !_x;
    _p.negative = _x & 0x80;
}
void Cpu::LDY(uint16_t address)
{
    _y = _bus->read(address);
    _p.zero = !_y;
    _p.negative = _y & 0x80;
}
void Cpu::LSR(uint16_t address)
{
    if (instruction_lookup_table[_opcode].addressing_mode == Accumulator) {
        _p.carry = _a & 0x1;
        _a >>= 1;
        _p.zero = !_a;
        _p.negative = _a & 0x80;
    } else {
        uByte temp = _bus->read(address);
        _p.carry = temp & 0x1;
        temp >>= 1;
        _p.zero = !temp;
        _p.negative = temp & 0x80;
        _bus->write(address, temp);
    }
}
void Cpu::NOP(uint16_t) {}
void Cpu::ORA(uint16_t address)
{
    _a |= _bus->read(address);
    _p.zero = !_a;
    _p.negative = _a & 0x80;
}
void Cpu::PHA(uint16_t) { push(_a); }
void Cpu::PHP(uint16_t) { push(_p.data); }
void Cpu::PLA(uint16_t)
{
    _a = pull();
    _p.zero = !_a;
    _p.negative = _a & 0x80;
}
void Cpu::PLP(uint16_t) { _p.data = pull(); }
void Cpu::ROL(uint16_t address)
{
    uByte old_carry = _p.carry;

    assert(old_carry == 1 || old_carry == 0);

    if (instruction_lookup_table[_opcode].addressing_mode == Accumulator) {
        _p.carry = _a & 0x80;
        _a = _a << 1 | old_carry;
        _p.zero = !_a;
        _p.negative = _a & 0x80;
    } else {
        uByte temp = _bus->read(address);
        _p.carry = temp & 0x80;
        temp = temp << 1 | old_carry;
        _p.zero = !temp;
        _p.negative = temp & 0x80;
    }
}
void Cpu::ROR(uint16_t address)
{
    uByte old_carry = _p.carry << 7;

    assert(old_carry == 0x80 || old_carry == 0);

    if (instruction_lookup_table[_opcode].addressing_mode == Accumulator) {
        _p.carry = _a & 0x1;
        _a = _a >> 1 | old_carry;
        _p.zero = !_a;
        _p.negative = _a & 0x80;
    } else {
        uByte temp = _bus->read(address);
        _p.carry = temp & 0x1;
        temp = temp >> 1 | old_carry;
        _p.zero = !temp;
        _p.negative = temp & 0x80;
    }
}
void Cpu::RTI(uint16_t)
{
    _p.data = pull();
    _pc = pull16();
}
void Cpu::RTS(uint16_t)
{
    // TODO: check this instruction size
    _pc = pull16() + 1;
    assert(strcmp(instruction_lookup_table[_bus->read(_pc) - 3].name, "JSR") ==
           0);
}
void Cpu::SBC(uint16_t address)
{
    // XXX: take care about this function
    uByte b = _bus->read(address);
    uint16_t result = _a - b - !_p.carry;

    _p.zero = !result;
    _p.negative = result & 0x80;
    _p.carry = result & 0x100;
    _p.overflow = (_a ^ result) & (b ^ result) & 0x80;
    _a = result;
}
void Cpu::SEC(uint16_t) { _p.carry = 1; }
void Cpu::SED(uint16_t) { _p.decimal = 1; }
void Cpu::SEI(uint16_t) { _p.interrupt_disable = 1; }
void Cpu::STA(uint16_t address) { _bus->write(address, _a); }
void Cpu::STX(uint16_t address) { _bus->write(address, _x); }
void Cpu::STY(uint16_t address) { _bus->write(address, _y); }
void Cpu::TAX(uint16_t)
{
    _x = _a;
    _p.zero = !_x;
    _p.negative = _x & 0x80;
}
void Cpu::TAY(uint16_t)
{
    _y = _a;
    _p.zero = !_y;
    _p.negative = _y & 0x80;
}
void Cpu::TSX(uint16_t)
{
    _x = _sp;
    _p.zero = !_x;
    _p.negative = _x & 0x80;
}
void Cpu::TXA(uint16_t)
{
    _a = _x;
    _p.zero = !_a;
    _p.negative = _a & 0x80;
}
void Cpu::TXS(uint16_t) { _sp = _x; }
void Cpu::TYA(uint16_t)
{
    _a = _y;
    _p.zero = !_a;
    _p.negative = _a & 0x80;
}
void Cpu::XXX(uint16_t) { throw runtime_error{"Cpu: Invalid opcode"}; }
