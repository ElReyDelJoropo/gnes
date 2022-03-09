#include "cpu.hpp"

using namespace gnes;

void Cpu::step() { handleInterrupt(); }
void Cpu::handleInterrupt() {
    uint16_t vector_address;

    //Should i put reset as interrupt type?
    if (_mmc.interrupt == INTERRUPT_TYPES::NONE)
        return;

    if (_mmc.interrupt == INTERRUPT_TYPES::IRQ) {
        if (_P.interrupt_disable)
            return;
        vector_address = IRQ_ADDRESS;
    } else {
        vector_address = NMI_ADDRESS;
    }

    _P.interrupt_disable = 1;
    push16(_PC);
    push(_P.data);
    _PC = _mmc.read16(vector_address);
    // XXX: this should be done here?
    _cycles += 7; // Cycles per interrupt handling
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
