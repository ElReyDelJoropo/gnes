#include "log_buffer.hpp"

#include <cassert>
#include <cmath>
using std::floor;
using std::log10;
#include <stdexcept>
using std::runtime_error;

#include <string_view>
using std::string_view;

using namespace gnes;

#ifdef NDEBUG
LogModule::LogModule() {}
LogModule::~LogModule() {}

void LogModule::SetBufferID(BufferID) {}

LogModule &LogModule::operator<<(int) { return *this; }
LogModule &LogModule::operator<<(string_view) { return *this; }

void LogModule::dump_buffer() {}

#else
LogModule::LogModule() : _current_resource_ptr(nullptr) {
    _resources[BufferID::CpuID].file.open("cpu.txt");
    _resources[BufferID::PpuID].file.open("ppu.txt");
    _resources[BufferID::ApuID].file.open("apu.txt");
    _resources[BufferID::CartrigdeID].file.open("cartrigde.txt");

    if (!_resources[BufferID::CpuID].file ||
        !_resources[BufferID::PpuID].file ||
        !_resources[BufferID::ApuID].file ||
        !_resources[BufferID::CartrigdeID].file)
        throw runtime_error{"Error: unable to create logfiles"};
}
LogModule::~LogModule() {
    for (int i = BufferID::CpuID; i <= BufferID::CartrigdeID; ++i) {
        _current_resource_ptr = &_resources[i];
        dump_buffer();
        _current_resource_ptr->file.close();
    }
}

void LogModule::setBufferID(BufferID id) {
    switch (id) {
    case BufferID::CpuID:
    case BufferID::PpuID:
    case BufferID::ApuID:
    case BufferID::CartrigdeID:
        _current_resource_ptr = &_resources[id];
        break;
    default:
        _current_resource_ptr = nullptr;
    }
}

LogModule &LogModule::operator<<(int i) {
    assert(_current_resource_ptr);
    _current_resource_ptr->buffer << i;
    return *this;
}
void LogModule::dump_buffer() {
    _current_resource_ptr->file << _current_resource_ptr->buffer.str();
}
#endif
