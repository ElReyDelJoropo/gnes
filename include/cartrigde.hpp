#pragma once
#include "interrupt_line.hpp"
#include "mapper.hpp"
#include "types.hpp"

#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace gnes {
struct RomInfo {
    int PRG_ROM_units;
    int CHR_ROM_units;
    int PRG_RAM_units;
    int mapper_number;
    int mirroring;
    bool has_trainer;
    bool has_PRG_RAM;
    bool uses_CHR_RAM;
    bool VS_Unisystem;
    bool play_choice_10;
    uByte nes_20_format;
    bool tv_system;
};

class Cartrigde {
  public:
    Cartrigde(InterruptLine &);
    void load(std::string &);

    uByte read(std::uint16_t);
    void write(std::uint16_t, uByte);

  private:
    InterruptLine &_interrupt_line;
    std::unique_ptr<Mapper> _mapper;
    RomInfo _rom_info;
    std::vector<uByte> _raw_data;

    int createMapper();
    int parseHeader(char[16]);
    std::ios::pos_type getRomSize(std::ifstream &);
};
} // namespace gnes
