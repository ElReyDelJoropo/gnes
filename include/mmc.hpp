#pragma once
#include "types.hpp"

namespace gnes {

class Mmc {
  public:
    ubyte read(std::uint16_t);
    std::uint16_t read16(std::uint16_t);
    void write(std::uint16_t, ubyte);

  private:
    ubyte _ram[0x800];

    void powerUp();
    
    void dma();

};
} // namespace gnes
