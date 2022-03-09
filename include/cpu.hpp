#pragma once

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
    IMPLICIT,
    ACCUMULATOR,
    IMMEDIATE,
    ABSOLUTE,
    RELATIVE,
    INDIRECT
};

class Cpu {
  public:
    Cpu(Mmc &);

    void step();

  private:
    static constexpr unsigned int CLOCK_FREQUENCY = 1789773;
        static constexpr std::uint16_t NMI_ADDRESS = 0xFFFA;
        static constexpr std::uint16_t RESET_ADDRESS = 0xFFFC;
        static constexpr std::uint16_t IRQ_ADDRESS = 0xFFFE;

    ubyte _X, _Y; // X and Y registers
    ubyte _SP;    // Stack pointer
    ubyte _A;     // Accumulator
    ubyte _PC;    // Program counter
    //XXX: An obscure but efficient approach
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

    int _cycles;
    Mmc &_mmc;

    void powerUp();

    void push(ubyte);
    void push16(std::uint16_t);

    ubyte pull();
    std::uint16_t pull16();

    void handleInterrupt();
};
} // namespace gnes
