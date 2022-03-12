#pragma once

#include "interrupt_line.hpp"
#include "mmc.hpp"
#include "types.hpp"

namespace gnes {

enum AddressingMode {
    ZeroPage,
    ZeroPageX,
    ZeroPageY,
    AbsoluteX,
    AbsoluteY,
    IndexedIndirect,
    IndirectIndexed,
    Implied,
    Accumulator,
    Immediate,
    Absolute,
    Relative,
    Indirect
};

class Cpu {
  public:
    Cpu(Mmc &, InterruptLine &);

    void step();

  private:
    static constexpr unsigned int CLOCK_FREQUENCY = 1789773;
    static constexpr std::uint16_t NMI_ADDRESS = 0xFFFA;
    static constexpr std::uint16_t RESET_ADDRESS = 0xFFFC;
    static constexpr std::uint16_t IRQ_ADDRESS = 0xFFFE;

    static const struct instruction {
        const char *name;
        void (Cpu::*func)(std::uint16_t);
        AddressingMode addressing_mode;
        int size;
        int cycle_lenght;
    } instruction_lookup_table[0xFF];

    uByte _X, _Y; // X and Y registers
    uByte _SP;    // Stack pointer
    uByte _A;     // Accumulator
    std::uint16_t _PC;    // Program counter
    // XXX: An obscure but efficient approach
    union {
        struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
            uByte carry : 1;
            uByte zero : 1;
            uByte interrupt_disable : 1;
            uByte decimal : 1;
            uByte brk : 1;
            uByte unused : 1;
            uByte overflow : 1;
            uByte negative : 1;
#else
            uByte negative : 1;
            uByte overflow : 1;
            uByte unused : 1;
            uByte brk : 1;
            uByte decimal : 1;
            uByte interrupt_disable : 1;
            uByte zero : 1;
            uByte carry : 1;
#endif
        };
        uByte data;
    } _P;

    uByte _opcode;

    int _cycles;
    Mmc &_mmc;
    InterruptLine &_interrupt_line;

    void powerUp();
    void reset();

    void push(uByte);
    void push16(std::uint16_t);

    uByte pull();
    std::uint16_t pull16();

    std::uint16_t translateAddress(AddressingMode);
    void handleInterrupt();

    //Instructions
    void ADC(std::uint16_t);
    void AND(std::uint16_t);
};
} // namespace gnes
