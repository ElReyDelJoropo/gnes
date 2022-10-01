#pragma once
#include "Cartrigde.hpp"
#include "types.hpp"

#include <array>

namespace gnes {

class CpuBus {
  public:
      CpuBus(Cartrigde *);
    uByte read(std::uint16_t address);
    std::uint16_t read16(std::uint16_t address);
    std::uint16_t read16Bug(std::uint16_t address);
    void write(std::uint16_t address, uByte b);

  private:
    std::array<uByte,0x800> _ram;
    Cartrigde *_cartrigde;

    void powerUp();
    void dma();

};
} // namespace gnes
