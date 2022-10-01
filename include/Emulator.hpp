#pragma once
#include "Cartrigde.hpp"
#include "Cpu.hpp"
#include "CpuBus.hpp"
#include "InterruptLine.hpp"
#include "LogModule.hpp"
#include "Ppu.hpp"
#include "VirtualScreen.hpp"

#include <filesystem>

namespace gnes{
    class Emulator{
        public:
        Emulator();
        void run(std::filesystem::path);
        private:
            CpuBus _cpu_bus;
            Cpu _cpu;
            Ppu _ppu;
            Cartrigde _cartrigde;
            InterruptLine _interrupt_line;
            LogModule _log_module;
            VirtualScreen _virtual_screen;

            void tick();
    };
}
