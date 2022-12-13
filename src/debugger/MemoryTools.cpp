#include "debugger/MemoryTools.hpp"
#include <array>
#include <cerrno>
#include <cstring>
#include <iomanip>
#include <stdexcept>
#include <string>

using namespace std;

namespace gnes::debug {
    MemoryTools::MemoryTools(std::array<uByte,0x800> *ram, std::array<uByte,0x1000> *vram): _ram(ram), _vram(vram){
        _vram_file.open("debug/MemoryTools.dir/vram.txt", ios::out);
        _ram_file.open("debug/MemoryTools.dir/ram.txt", ios::out);

        if (!(_vram_file && _ram_file))
            throw std::runtime_error{string("MemoryTools: Unable to create dump files") + strerror(errno)};

        dumpRam();
        dumpVram();
    }
    uByte MemoryTools::readFromRam(std::uint16_t address){
        return _ram->at(address & 0x7ff);
    }
    uByte MemoryTools::readFromVram(std::uint16_t address){
        return _vram->at(address);
    }

    void MemoryTools::dumpRam(){
        for (int i = 0; i != 0x80; ++i) {
            for (int j = 0; j != 0x10; ++j)
                _ram_file << setw(2) << setfill('0') << (int)_ram->at(i*j);
            _ram_file << '\n';
        }
    }
    void MemoryTools::dumpVram(){
        for (int i = 0; i != 0x100; ++i) {
            for (int j = 0; j != 0x10; ++j)
                _vram_file << setw(2) << setfill('0') << (int)_vram->at(i*j);
            _vram_file << '\n';
        }
    }

}
