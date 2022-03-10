#pragma once

#include "interrupt_line.hpp"
#include "mmc.hpp"
#include "types.hpp"

namespace gnes {

enum ADDRESSING_MODE {
    ZERO_PAGE,
    ZERO_PAGE_X,
    ZERO_PAGE_Y,
    ABSOLUTE_X,
    ABSOLUTE_Y,
    INDEXED_INDIRECT,
    INDIRECT_INDEXED,
    IMPLIED,
    ACCUMULATOR,
    IMMEDIATE,
    ABSOLUTE,
    RELATIVE,
    INDIRECT
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
        ADDRESSING_MODE addressing_mode;
        int size;
        int cycle_lenght;
    } instruction_lookup_table[0xFF];

    ubyte _X, _Y; // X and Y registers
    ubyte _SP;    // Stack pointer
    ubyte _A;     // Accumulator
    std::uint16_t _PC;    // Program counter
    // XXX: An obscure but efficient approach
    union {
        struct {
            ubyte carry : 1;
            ubyte zero : 1;
            ubyte interrupt_disable : 1;
            ubyte decimal : 1;
            ubyte brk : 1;
            ubyte unused : 1;
            ubyte overflow : 1;
            ubyte negative : 1;
        };
        ubyte data;
    } _P;

    ubyte _opcode;

    int _cycles;
    Mmc &_mmc;
    InterruptLine &_interrupt_line;

    void powerUp();
    void reset();

    void push(ubyte);
    void push16(std::uint16_t);

    ubyte pull();
    std::uint16_t pull16();

    std::uint16_t translateAddress(ADDRESSING_MODE);
    void handleInterrupt();
};
} // namespace gnes
