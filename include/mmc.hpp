#pragma once
#include "types.hpp"

namespace gnes {

class Mmc {
  public:
    uByte read(std::uint16_t);
    std::uint16_t read16(std::uint16_t);
    void write(std::uint16_t, uByte);

  private:
    uByte _ram[0x800];

    void powerUp();
    
    void dma();

};
} // namespace gnes
