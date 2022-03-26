#pragma once
#include "Cpu.hpp"
#include "Cartrigde.hpp"
#include "InterruptLine.hpp"
#include "LogModule.hpp"
#include "CpuBus.hpp"

#include <filesystem>

namespace gnes{
    class Emulator{
        public:
        Emulator();
        void run(std::filesystem::path);
        private:
            CpuBus _cpu_bus;
            Cpu _cpu;
            Cartrigde _cartrigde;
            InterruptLine _interrupt_line;
            LogModule _log_module;

            void tick();
    };
}
