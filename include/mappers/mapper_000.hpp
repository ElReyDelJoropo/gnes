#pragma once

#include "interrupt_line.hpp"
#include "mapper.hpp"

#include <vector>

namespace gnes {

class Mapper_000 final : public Mapper {
  public:
    Mapper_000(RomInfo *, std::vector<uByte>);
    std::uint16_t read(std::uint16_t address) const override;
    void write(std::uint16_t address, uByte b) override;
    std::uint16_t get_PagePtr(std::uint16_t address) const override;

  private:
    RomInfo *_rom_info;
    std::vector<uByte> _rom_data;
    std::vector<uByte> _prg_ram;
    std::uint16_t mask;
};

} // namespace gnes
