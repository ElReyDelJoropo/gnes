#include "Cpu.hpp"
#include "Debugger.hpp"
#include "debugger/LogModule.hpp"
#include "types.hpp"

#include <sstream>
using std::ostringstream;

using namespace gnes;


Debugger::Debugger(Cpu *cpu, Ppu *ppu, Cartrigde *cartrigde): _cpu(cpu), _ppu(ppu), _cartrigde(cartrigde), _memory_tools(&_cpu->_bus->_ram,&_ppu->_bus._vram){}
    ostringstream &Debugger::cpuStream(){
        return _log_module.getBuffer(debug::BufferID::CpuID);
    } 
    ostringstream &Debugger::cartrigdeStream(){
        return _log_module.getBuffer(debug::BufferID::CartrigdeID);
    } 
    // uByte Debugger::peek(std::uint16_t address){
    //     return _memory_tools.readFromRam(address);
    // }

