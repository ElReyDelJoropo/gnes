#pragma once
#include "types.hpp"

#include <array>
#include <fstream>
#include <sstream>
#include <string_view>

namespace gnes {
// xxenum class BufferID {Cpu, Ppu, Apu, Cartrigde, None };

enum BufferID { None = -1, CpuID, PpuID, ApuID, CartrigdeID };

class LogModule {
  public:
    LogModule();
    ~LogModule();

    void setBufferID(BufferID id);
    LogModule &operator<<(int);
    LogModule &operator<<(std::string_view);

  private:
#ifndef NDEBUG
    static constexpr std::streamsize BUFFER_MAX_SIZE = 4096;
    struct LogResource {
        std::ostringstream buffer;
        std::ofstream file;
        int count;
    };

    LogResource _resources[4];
    LogResource *_current_resource_ptr;
#endif
    void dump_buffer();
};
} // namespace gnes
