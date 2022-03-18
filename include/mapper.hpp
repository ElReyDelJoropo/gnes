#pragma once
#include "interrupt_line.hpp"
#include "types.hpp"

#include <vector>

namespace gnes {

struct RomInfo;

class Mapper {
  public:
    virtual ~Mapper() = default;
    virtual std::uint16_t read(uint16_t) const = 0;
    virtual void write(uint16_t, uByte) = 0;
    virtual std::uint16_t get_PagePtr(uint16_t) const = 0;

  private:
    static constexpr int chr_rom_unit_size = 16384;
};
} // namespace gnes
