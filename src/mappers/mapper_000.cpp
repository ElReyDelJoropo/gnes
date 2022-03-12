#include "mappers/mapper_000.hpp"
#include "cartrigde.hpp"
#include "interrupt_line.hpp"

#include <cstdint>
using std::uint16_t;
#include <cstdio>
using std::fprintf;
#include <vector>

using namespace gnes;

Mapper_000::Mapper_000(RomInfo *info, InterruptLine &il,
                       std::vector<uByte> const &v)
    : _rom_info(info), _interrupt_line(il), _rom_data(v) {}

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

void Mapper_000::write(uint16_t address, uByte b){
    if (address < 0x8000){
        //TODO: access to PRG_RAM
    }   
    else {
        fprintf(stderr,"An error had ocurred: attemped to write on read only memory at address %X",address);
        _interrupt_line.setInterrupt(InterruptType::Error);
    }
}

