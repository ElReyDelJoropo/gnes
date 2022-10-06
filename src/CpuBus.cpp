#include "CpuBus.hpp"
#include "Cartrigde.hpp"

#include <cstdint>
#include <cstring>
using std::uint16_t;

using namespace gnes;

CpuBus::CpuBus(Cartrigde *c, Ppu *p) : _ram{}, _cartrigde(c), _ppu(p) { powerUp(); }
void CpuBus::powerUp() {}
uByte CpuBus::read(uint16_t address)
{
    switch (address) {
    // Internal ram
    case 0x0 ... 0x1FFF:
        return _ram[address & 0x7FF];
    // PPU registers
    case 0x2000 ... 0x3FFF:
        return _ppu->read(address & 0x2007);
    // APU
    case 0x4000 ... 0x401F:
        break;
    // Cartrigde space
    case 0x4020 ... 0xFFFF:
        return _cartrigde->readPrg(address);
    default:
        break;
    }

    return 0;
}
uint16_t CpuBus::read16(uint16_t address)
{
    uint16_t low = read(address);
    uint16_t high = read(address + 1);

    return high << 8 | low;
}
uint16_t CpuBus::read16Bug(uint16_t address)
{
    uint16_t page = address & 0xFF00;
    uint16_t low = read(address);
    uint16_t high = read(page | (address + 1 & 0xFF));

    return high << 8 | low;
}
void CpuBus::write(uint16_t address, uByte b){
    switch (address) {
    // Internal ram
    case 0x0 ... 0x1FFF:
        _ram[address & 0x7FF] = b;
    // PPU registers
    case 0x2000 ... 0x3FFF:
        _ppu->write(address,b);
    // APU
    case 0x4000 ... 0x401F:
        break;
    // Cartrigde space
    case 0x4020 ... 0xFFFF:
        _cartrigde->writePrg(address,b);
    default:
        break;
    }
}
