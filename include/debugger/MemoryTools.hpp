#pragma once
#include <array>
#include <fstream>
#include "types.hpp"
#include <cstdint>

namespace gnes::debug {
class MemoryTools{
    public:
        MemoryTools(std::array<uByte,0x800> *ram,std::array<uByte,0x1000> *vram);

        uByte readFromRam(std::uint16_t address);
        uByte readFromVram(std::uint16_t address);

        // void updateRam(std::uint16_t address);
        // void updateVram(std::uint16_t address);

    private:
        std::fstream _ram_file;
        std::fstream _vram_file;

        std::array<uByte,0x800> *_ram;
        std::array<uByte,0x1000> *_vram;

        void dumpRam();
        void dumpVram();
};
}
