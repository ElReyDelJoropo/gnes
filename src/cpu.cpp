#include "cpu.hpp"

using namespace gnes;

void Cpu::powerUp() {
    _P.data = 0x34;
    _A = _X = _Y = 0;
    _SP = 0xFD;
}
void Cpu::reset() {
    // TODO: silence APU
    _interrupt_line.setInterrupt(RESET);
}
void Cpu::step() {
    uint16_t address;

    handleInterrupt();

    _opcode = _mmc.read(_PC++);
    address =
        translateAddress(instruction_lookup_table[_opcode].addressing_mode);
    (this->*instruction_lookup_table[_opcode].func)(address);
    _PC += instruction_lookup_table[_opcode].size;
    _cycles += instruction_lookup_table[_opcode].cycle_lenght;
}

uint16_t Cpu::translateAddress(ADDRESSING_MODE mode) {
    //_PC will pointing byte after opcode
    switch (mode) {
    // Zero page and relative addressing fetch the address in the same way, but
    // relative will use this operand as signed integer
    case ZERO_PAGE:
    case RELATIVE:
        return _mmc.read(_PC);
    case ZERO_PAGE_X:
        return _mmc.read(_PC) + _X & 0xFF; // Wrapped around
    case ZERO_PAGE_Y:
        return _mmc.read(_PC) + _Y & 0xFF;
    case ABSOLUTE:
        return _mmc.read16(_PC);
    case ABSOLUTE_X:
        return _mmc.read16(_PC) + _X;
    case ABSOLUTE_Y:
        return _mmc.read16(_PC) + _Y;
    case INDIRECT:
        return _mmc.read16(_mmc.read16(_PC));
    case INDEXED_INDIRECT:
        return _mmc.read16(_mmc.read(_PC) + _X);
    case INDIRECT_INDEXED:
        return _mmc.read16(_mmc.read(_PC)) + _Y;
    case IMPLIED:
    case ACCUMULATOR:
    case IMMEDIATE:
        return 0;
    }
}
void Cpu::handleInterrupt() {
    uint16_t vector_address;
    INTERRUPT_TYPES interrupt = _interrupt_line.getInterrupt();
    // Should i put reset as interrupt type?
    if (interrupt == INTERRUPT_TYPES::NONE)
        return;

    if (interrupt == INTERRUPT_TYPES::IRQ) {
        if (_P.interrupt_disable)
            return;
        vector_address = IRQ_ADDRESS;
    } else if (interrupt == INTERRUPT_TYPES::NMI) {
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

void Cpu::push(ubyte b) {
    _mmc.write(_SP | 0x100, b);
    --_SP; // Stack grows downward
}
void Cpu::push16(uint16_t b) {
    push(b >> 8);
    push(b);
}

ubyte Cpu::pull() { return _mmc.read(++_SP | 0x100); }
uint16_t Cpu::pull16() {
    uint16_t low = pull();
    uint16_t high = pull();

    return high << 8 | low;
}
