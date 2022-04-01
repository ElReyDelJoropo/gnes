#pragma once
#include "InterruptLine.hpp"
#include "types.hpp"

#include <vector>

namespace gnes {

struct RomInfo;

class Mapper {
  public:
    virtual ~Mapper() = default;
    [[nodiscard]] virtual std::uint16_t readPrg(uint16_t) const = 0;
    [[nodiscard]] virtual std::uint16_t readChr(uint16_t) const = 0;
    virtual void writePrg(uint16_t, uByte) = 0;
    virtual void writeChr(uint16_t, uByte) = 0;
    [[nodiscard]] virtual std::uint16_t get_PagePtr(uint16_t) const = 0;

  private:
    static constexpr int chr_rom_unit_size = 16384;
};
} // namespace gnes
