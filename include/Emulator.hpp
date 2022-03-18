#pragma once
#include "cpu.hpp"
#include "cartrigde.hpp"
#include "interrupt_line.hpp"
#include "log_buffer.hpp"
#include "mmc.hpp"

#include <filesystem>

namespace gnes{
    class Emulator{
        public:
        Emulator();
        void run(std::filesystem::path);
        private:
            Mmc _mmc;
            Cpu _cpu;
            Cartrigde _cartrigde;
            InterruptLine _interrupt_line;
            LogModule _log_module;

            void tick();
    };
}
