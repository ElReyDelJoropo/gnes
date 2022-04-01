#pragma once
#include <array>

#include "Cartrigde.hpp"
#include "types.hpp"

namespace gnes {


class PpuBus {
    public:
        PpuBus(Cartrigde *c);
        [[nodiscard]] uByte read(std::uint16_t address) const;
        void write(std::uint16_t address, uByte b);
  private:
    std::array<uByte,0x1000> _vram;
    std::array<uByte,0x10> _palette;
    Cartrigde *_cartrigde;
};
} // namespace gnes
