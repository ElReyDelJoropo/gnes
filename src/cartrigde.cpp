#include "cartrigde.hpp"
#include "mapper.hpp"
#include "mappers/mapper_000.hpp"

#include <cerrno>
#include <cstdint>
using std::uint16_t;
#include <cstring>
using std::strerror;
#include <filesystem>
using std::filesystem::path;
#include <fstream>
using std::ifstream;
#include <memory>
using std::make_unique;
#include <stdexcept>
using std::runtime_error;

using namespace gnes;

uByte Cartrigde::read(uint16_t a) { return _mapper->read(a); }
void Cartrigde::write(uint16_t a, uByte b) { _mapper->write(a, b); }

void Cartrigde::load(path &game_path)
{
    ifstream file;
    char header[16];

    file.open(game_path, std::ios::in | std::ios::binary);

    if (!file)
        throw runtime_error(strerror(errno));

    auto sz = std::filesystem::file_size(game_path);

    if (sz < 16)
        throw runtime_error("corrupted rom");

    file.read(header, 16);

    if (parseHeader(header) == -1)
        throw runtime_error("invalid header");

    _raw_data.resize(sz);
    // XXX: Could I avoid this cast?
    file.read(reinterpret_cast<ifstream::char_type *>(_raw_data.data()), sz);
    createMapper();

    if (!_mapper)
        throw runtime_error("Mapper #" +
                            std::to_string(_rom_info.mapper_number) +
                            " not supported");
}

int Cartrigde::parseHeader(char header[16])
{
    if (strncmp(header, "NES\x1A", 4) != 0) {
        return -1;
    }

    _rom_info.PRG_ROM_units = header[4];
    _rom_info.CHR_ROM_units = header[5];
    _rom_info.has_PRG_RAM = header[6] & 0x2;
    _rom_info.has_trainer = header[6] & 0x4;
    _rom_info.mirroring = header[6] & 0x9;
    _rom_info.mapper_number = (header[7] & 0xF0) | ((header[6] & 0xF0) >> 4);
    _rom_info.VS_Unisystem = header[7] & 0x1;
    _rom_info.play_choice_10 = header[7] & 0x2;
    _rom_info.nes_20_format = header[7] & 0xC;
    _rom_info.PRG_RAM_units = header[8] ? header[8] : 1;

    return 0;
}
void Cartrigde::createMapper()
{
    switch (_rom_info.mapper_number) {
    case 0:
        _mapper =
            make_unique<Mapper_000>(&_rom_info, _interrupt_line, _raw_data);
        break;
    default:
        _mapper = nullptr;
        break;
    }
}
