#include "Debugger.hpp"
#include "debugger/LogModule.hpp"

#include <sstream>
using std::ostringstream;


namespace gnes{
    Debugger::Debugger(Cpu *cpu, Cartrigde *crt): _cpu(cpu), _cartrigde(crt){}
    ostringstream &Debugger::cpuStream(){
        return _log_module.getBuffer(BufferID::CpuID);
    } 
    ostringstream &Debugger::cartrigdeStream(){
        return _log_module.getBuffer(BufferID::CartrigdeID);
    } 
}
