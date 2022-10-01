#pragma once

#include "InterruptLine.hpp"
#include "Mapper.hpp"

#include <vector>

namespace gnes {

class Mapper_000 final : public Mapper {
  public:
    Mapper_000(RomInfo *, std::vector<uByte> const &);
    std::uint16_t readPrg(std::uint16_t address) const override;
    std::uint16_t readChr(std::uint16_t address) const override;
    void writePrg(std::uint16_t address, uByte b) override;
    void writeChr(std::uint16_t address, uByte b) override;
    std::uint16_t get_PagePtr(std::uint16_t address) const override;

  private:
    RomInfo *_rom_info;
    std::vector<uByte> _prg_rom;
    std::vector<uByte> _chr_rom;
    std::vector<uByte> _prg_ram;
    std::uint16_t mask;
};

} // namespace gnes
