#pragma once
#include "types.hpp"

#include <cassert>
#include <fstream>
#include <sstream>

namespace gnes {
// enum class BufferID {Cpu, Ppu, Apu, Cartrigde, None };

enum BufferID {CpuID, PpuID, ApuID, CartrigdeID };

#ifndef NDEBUG

class LogModule {
  public:
    LogModule()
    {
        _resources[BufferID::CpuID].file.open("cpu.txt");
        _resources[BufferID::PpuID].file.open("ppu.txt");
        _resources[BufferID::ApuID].file.open("apu.txt");
        _resources[BufferID::CartrigdeID].file.open("cartrigde.txt");

        if (!_resources[BufferID::CpuID].file ||
            !_resources[BufferID::PpuID].file ||
            !_resources[BufferID::ApuID].file ||
            !_resources[BufferID::CartrigdeID].file)
            throw std::runtime_error{"LogModule: unable to create logfiles"};
    }
    ~LogModule(){
        for (auto &resource : _resources)
            resource.file << resource.buffer.str();
        
    }
    std::ostringstream& getBuffer(BufferID id){
        return _resources[static_cast<int>(id)].buffer;
    }
  private:
    struct {
        std::ostringstream buffer;
        std::ofstream file;
    } _resources[4];

};
#else

class LogModule {
    LogModule() = default;
};
#endif
} // namespace gnes
