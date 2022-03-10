#pragma once

#include "mapper.hpp"

#include <vector>

namespace gnes {

class Mapper_000 final: public Mapper {
  public:
    Mapper_000(RomInfo *, std::vector<byte> const &);
    virtual ~Mapper_000();
    virtual std::uint16_t read(std::uint16_t);
    virtual void write(std::uint16_t, ubyte);
    virtual std::uint16_t get_PagePtr(std::uint16_t);
    
  private:
    RomInfo *_rom_info;
    std::vector<byte> _rom_data;
    
};

} // namespace gnes
