#include "mappers/mapper_000.hpp"
#include "cartrigde.hpp"

#include <cstdint>
using std::uint16_t;
#include <vector>

using namespace gnes;

Mapper_000::Mapper_000(RomInfo *info, std::vector<byte> const &v)
    : _rom_info(info), _rom_data(v) {}

uint16_t Mapper_000::read(uint16_t address) {
    uint16_t ret;
    if (address < 0x8000) {
        // TODO: access to PRG_RAM
        ret = 0;
    } else if (_rom_info->PRG_ROM_units == 1) {
        ret = _rom_data.at(address - 0x8000 & 0xBFFF); // Mirrored
    } else {
        ret = _rom_data.at(address - 0x8000);
    }
    return ret;
}

