#pragma once
#include "interrupt_line.hpp"
#include "types.hpp"

#include <vector>

namespace gnes {

struct RomInfo;

class Mapper {
  public:
    virtual ~Mapper() {}
    virtual std::uint16_t read(uint16_t) = 0;
    virtual void write(uint16_t, ubyte) = 0;
    virtual std::uint16_t get_PagePtr(uint16_t) = 0;
};
} // namespace gnes
