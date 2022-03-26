#include "mappers/Mapper_000.hpp"
#include "Cartrigde.hpp"
#include "InterruptLine.hpp"

#include <algorithm>
using std::advance;
using std::copy_n;
#include <cstdint>
using std::uint16_t;
#include <iterator>
using std::back_inserter;
#include <stdexcept>
using std::runtime_error;
#include <string>
using std::to_string;
#include <vector>
using std::vector;

using namespace gnes;

Mapper_000::Mapper_000(RomInfo *info, vector<uByte> const &v)
    : _rom_info(info), _prg_ram(0x2000, 0x00),
      mask(info->PRG_ROM_units == 1 ? 0xBFFF : 0xFFFF)
{
    auto iter = copy_n(v.begin(), _rom_info->PRG_ROM_units * 16384,
                       back_inserter(_prg_rom));
    copy_n(iter, _rom_info->CHR_ROM_units * /*FIXME*/ 2048,
           back_inserter(_chr_rom));
}

uint16_t Mapper_000::readPrg(uint16_t address) const
{
    return _prg_rom.at(address - 0x8000 & mask);
}
uint16_t Mapper_000::readChr(uint16_t address) const
{
    return _chr_rom.at(address);
}
void Mapper_000::writePrg(uint16_t address, uByte)
{
    throw runtime_error{"Mapper: Attemp to modify prg rom data at address " +
                        to_string(address)};
}
uint16_t Mapper_000::get_PagePtr(uint16_t address) const
{
    // TODO: complete this function
    return address;
}

