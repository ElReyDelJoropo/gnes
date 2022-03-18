#include "mmc.hpp"
#include "cartrigde.hpp"

#include <cstdint>
#include <cstring>
using std::uint16_t;

using namespace gnes;

Mmc::Mmc(Cartrigde &c) : _cartrigde(c) { powerUp(); }
void Mmc::powerUp() { std::memset(_ram, 0x0, 0x800); }
uByte Mmc::read(uint16_t address)
{
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
        return _cartrigde.read(address);
    default:
        break;
    }

    return 0;
}
uint16_t Mmc::read16(uint16_t address)
{
    uint16_t low = read(address);
    uint16_t high = read(address + 1);

    return high << 8 | low;
}
uint16_t Mmc::read16Bug(uint16_t address)
{
    uint16_t page = address & 0xFF00;
    uint16_t low = read(address);
    uint16_t high = read(page | (address + 1 & 0xFF));

    return high << 8 | low;
}
