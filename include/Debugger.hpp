#pragma once
#include "types.hpp"
#include "debugger/LogModule.hpp"

#include <sstream>

namespace gnes {

class Cpu;
class Cartrigde;

class Debugger {
    public:
        Debugger(Cpu *, Cartrigde *);
        std::ostringstream &cpuStream();
        std::ostringstream &cartrigdeStream();
    private:
        LogModule _log_module;        
        Cpu *_cpu;
        Cartrigde *_cartrigde;
};
}
