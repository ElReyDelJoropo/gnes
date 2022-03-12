#pragma once

#include "interrupt_line.hpp"
#include "mapper.hpp"

#include <vector>

namespace gnes {

class Mapper_000 final : public Mapper {
  public:
    Mapper_000(RomInfo *, InterruptLine &, std::vector<uByte> const &);
    virtual ~Mapper_000();
    virtual std::uint16_t read(std::uint16_t);
    virtual void write(std::uint16_t, uByte);
    virtual std::uint16_t get_PagePtr(std::uint16_t);

  private:
    RomInfo *_rom_info;
    InterruptLine &_interrupt_line;
    std::vector<uByte> _rom_data;
};

} // namespace gnes
