#pragma once
#include "cartrigde.hpp"
#include "types.hpp"

#include <array>

namespace gnes {

class Mmc {
  public:
      Mmc(Cartrigde &);
    uByte read(std::uint16_t);
    std::uint16_t read16(std::uint16_t);
    std::uint16_t read16Bug(std::uint16_t);
    void write(std::uint16_t, uByte);

  private:
    std::array<uByte,0x800> _ram;
    Cartrigde &_cartrigde;

    void powerUp();
    
    void dma();

};
} // namespace gnes
