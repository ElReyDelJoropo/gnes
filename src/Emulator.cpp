#include "Emulator.hpp"

#include <filesystem>
using std::filesystem::path;

using namespace gnes;

Emulator::Emulator(): _mmc(&_cartrigde), _cpu(&_mmc,&_interrupt_line,&_log_module), _cartrigde(&_interrupt_line,&_log_module){

}

void Emulator::run(path p){
    _cartrigde.load(p);
    _cpu.powerUp();
    _cpu.reset();
    for (int i = 0; i != 100; ++i){
        tick();
    }
}

void Emulator::tick(){
    _cpu.step();
}
