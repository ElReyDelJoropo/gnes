#include <stdexcept>
using std::runtime_error;
#include <string>
using std::string;
using std::to_string;

#include "PpuBus.hpp"
#include "types.hpp"
using std::uint16_t;

using namespace gnes;

PpuBus::PpuBus(Cartrigde *c): _cartrigde(c){}
uByte PpuBus::read(uint16_t address) const
{
    switch (address) {
    case 0x0 ... 0x1FFF:
        return _cartrigde->readChr(address);
    case 0x2000 ... 0x3EFF:
        return _vram.at(address & 0x3000);
    case 0x3F00 ... 0x3FFF:
        return 1; // TODO: read palette
    default:
        throw runtime_error{"PpuBus: Address out of range " +
                            to_string(address)};
    }
}
void PpuBus::write(uint16_t address, uByte b)
{
    switch (address) {
    case 0x0 ... 0x1FFF:
        _cartrigde->writeChr(address, b);
        break;
    case 0x2000 ... 0x3EFF:
        _vram.at(address) = b;
        break;
    case 0x3F00 ... 0x3FFF:
        break;
        // TODO: read palette
    default:
        throw runtime_error{"PpuBus: Address out of range " +
                            to_string(address)};
    }
}
