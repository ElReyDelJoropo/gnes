#pragma once
#include "types.hpp"

#include <cassert>
#include <fstream>
#include <sstream>

namespace gnes {
// enum class BufferID {Cpu, Ppu, Apu, Cartrigde, None };

enum BufferID { None = -1, CpuID, PpuID, ApuID, CartrigdeID };

#ifndef NDEBUG

class LogModule {
  public:
    LogModule():_current_resource_ptr(nullptr)
    {
        _resources[BufferID::CpuID].file.open("cpu.txt");
        _resources[BufferID::PpuID].file.open("ppu.txt");
        _resources[BufferID::ApuID].file.open("apu.txt");
        _resources[BufferID::CartrigdeID].file.open("cartrigde.txt");

        if (!_resources[BufferID::CpuID].file ||
            !_resources[BufferID::PpuID].file ||
            !_resources[BufferID::ApuID].file ||
            !_resources[BufferID::CartrigdeID].file)
            throw std::runtime_error{"Error: unable to create logfiles"};
    }
    ~LogModule()
    {
        for (int i = BufferID::CpuID; i <= BufferID::CartrigdeID; ++i) {
            _resources[i].file << _resources[i].buffer.str();
            _current_resource_ptr->file.close();
        }
    }
    void setBufferID(BufferID id)
    {
        _current_resource_ptr = &_resources[static_cast<int>(id)];
    }

    template <typename T> LogModule &operator<<(T val)
    {
        assert(_current_resource_ptr);
        _current_resource_ptr->buffer << val;
        return *this;
    }

  private:
    static constexpr std::streamsize BUFFER_MAX_SIZE = 4096;
    struct LogResource {
        std::ostringstream buffer;
        std::ofstream file;
    };

    LogResource _resources[4];
    LogResource *_current_resource_ptr;
};
#else

class LogModule {
    LogModule() = default;
    ~LogModule() = default;
    void setBufferID(BufferID id) {}

    template<typename T>
    LogModule &operator<<(T) {}
};
#endif
} // namespace gnes
