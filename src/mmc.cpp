#include "mmc.hpp"

#include <cstring>
#include <cstdint>
using std::uint16_t;

using namespace gnes;

void Mmc::powerUp(){
    std::memset(_ram,0x0,0x800);
}
ubyte Mmc::read(uint16_t address) {
    switch (address) {
    // Internal ram
    case 0x0 ... 0x1FFF:
        return _ram[address & 0x7FF];
    // PPU registers
    case 0x2000 ... 0x3FFF:
        break;
    // APU
    case 0x4000 ... 0x401F:
        break;
    // Cartrigde space
    case 0x4020 ... 0xFFFF:
        break;
    default:
        break;
    }

    return 0;
}
uint16_t Mmc::read16(uint16_t address) {
    uint16_t low = read(address);
    uint16_t high = read(address + 1);

    return high << 8 | low;
}
