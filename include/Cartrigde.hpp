#pragma once
#include "InterruptLine.hpp"
#include "Mapper.hpp"
#include "LogModule.hpp"
#include "types.hpp"

#include <filesystem>
#include <fstream>
#include <memory>
#include <vector>

namespace gnes {
struct RomInfo {
    int PRG_ROM_units;
    int CHR_ROM_units;
    int PRG_RAM_units;
    int mapper_number;
    int submapper_number;
    int mirroring;
    bool four_screen_mode;
    bool has_trainer;
    bool has_PRG_RAM;
    bool uses_CHR_RAM;
    bool VS_Unisystem;
    bool play_choice_10;
    bool is_nes_20_format;
    bool tv_system;
};

class Cartrigde {
  public:
    Cartrigde(InterruptLine *, LogModule *);
    void load(std::filesystem::path &);

    uByte read(std::uint16_t address);
    void write(std::uint16_t address, uByte b);

  private:
    InterruptLine *_interrupt_line;
    LogModule *_log_module;
    std::unique_ptr<Mapper> _mapper;
    RomInfo _rom_info;
    std::vector<uByte> _raw_data;

    int parseHeader(char h[16]);
    std::unique_ptr<Mapper> makeMapper();
    void dumpRomInfo();
};
} // namespace gnes
