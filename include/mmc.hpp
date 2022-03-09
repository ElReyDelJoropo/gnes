#pragma once
#include "types.hpp"

namespace gnes {
enum INTERRUPT_TYPES { NONE, IRQ, BRK, NMI };

class Mmc {
  public:
    ubyte read(std::uint16_t);
    std::uint16_t read16(std::uint16_t);
    void write(std::uint16_t, ubyte);

    INTERRUPT_TYPES interrupt;

  private:
    ubyte _ram[0x800];

    void dma();
};
} // namespace gnes
