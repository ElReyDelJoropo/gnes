#pragma once
#include "types.hpp"
#include "debugger/nesfwd.hpp"
#include "debugger/LogModule.hpp"
#include "debugger/MemoryTools.hpp"

#include <array>
#include <cstdint>
#include <sstream>

namespace gnes {

class Debugger {
    public:
        Debugger(Cpu *cpu, Ppu *ppu, Cartrigde *cartrigde);
        std::ostringstream &cpuStream();
        std::ostringstream &cartrigdeStream();

        void dumpRam(); //MemoryTools
        uByte peek(std::uint16_t address); //MemoryTools
        void updateRam(std::uint16_t address, uByte b);
        void updateVram(std::uint16_t address, uByte b);
    private:
        Cpu *_cpu;
        Ppu *_ppu;
        Cartrigde *_cartrigde;
        debug::LogModule _log_module;        
        debug::MemoryTools _memory_tools;

        void testing(uByte *);
};
}
