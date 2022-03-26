#include "mappers/mapper_000.hpp"
#include "cartrigde.hpp"
#include "interrupt_line.hpp"

#include <cstdint>
using std::uint16_t;
#include <stdexcept>
using std::runtime_error;
#include <vector>

using namespace gnes;

Mapper_000::Mapper_000(RomInfo *info, std::vector<uByte> v)
    : _rom_info(info), _rom_data(std::move(v)), _prg_ram(0x2000, 0x00),
      mask(info->PRG_ROM_units == 1 ? 0xBFFF : 0xFFFF)
{
}

uint16_t Mapper_000::read(uint16_t address) const
{
    return address < 0x8000 ? _prg_ram.at(address - 0x6000)
                            : _rom_data.at(address - 0x8000 & mask);
}

void Mapper_000::write(uint16_t address, uByte b)
{
    if (address >= 0x8000)
        throw runtime_error{"Mapper: Attemp to modify a read only area"};
    
    _prg_ram.at(address-0x6000) = b;
}

uint16_t Mapper_000::get_PagePtr(uint16_t address) const{
    //TODO: complete this function
    return address;
}

