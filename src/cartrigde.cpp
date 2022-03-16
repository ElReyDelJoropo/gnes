#include "cartrigde.hpp"
#include "log_buffer.hpp"
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
#include <string>

using namespace gnes;

Cartrigde::Cartrigde(InterruptLine &line, LogModule &logm)
    : _interrupt_line(line), _log_module(logm)
{
}

uByte Cartrigde::read(uint16_t a) { return _mapper->read(a); }
void Cartrigde::write(uint16_t a, uByte b) { _mapper->write(a, b); }

void Cartrigde::load(path &game_path)
{
    ifstream file{game_path, std::ios::in | std::ios::binary};
    char header[16];

    if (!file)
        throw runtime_error{std::string{"Cartrigde: "} + strerror(errno)};

    if (!file.read(header, 16))
        throw runtime_error{"Cartrigde: Corrupted rom"};

    if (parseHeader(header) == -1)
        throw runtime_error{"Cartrigde: Invalid header"};

    auto sz = std::filesystem::file_size(game_path) - 16;
    _raw_data.resize(sz);

    if (!file.read(reinterpret_cast<ifstream::char_type *>(_raw_data.data()),
                   sz))
        throw runtime_error{"Cartrigde: Unable to read the rom"};

    _mapper = makeMapper();
    if (!_mapper)
        throw runtime_error{"Cartrigde: Mapper #" +
                            std::to_string(_rom_info.mapper_number) +
                            " not supported"};
    dumpRomInfo();
}

int Cartrigde::parseHeader(char header[16])
{
    if (strncmp(header, "NES\x1A", 4) != 0) {
        return -1;
    }

    //FIXME: improve parsing the header
    _rom_info.PRG_ROM_units = header[4];
    _rom_info.CHR_ROM_units = header[5];
    _rom_info.uses_CHR_RAM = !_rom_info.CHR_ROM_units;    
    _rom_info.has_PRG_RAM = header[6] & 0x2;
    _rom_info.has_trainer = header[6] & 0x4;
    _rom_info.four_screen_mode = header[6] & 0x8;
    _rom_info.mirroring = header[6] & 0x9 ? 1 : 0;
    _rom_info.mapper_number = (header[7] & 0xF0) | ((header[6] & 0xF0) >> 4);
    _rom_info.VS_Unisystem = header[7] & 0x1;
    _rom_info.play_choice_10 = header[7] & 0x2;
    _rom_info.is_nes_20_format = (header[7] & 0xC) == 0x8;
    //_rom_info.PRG_RAM_units = header[8] ? header[8] : 1;
    
    if (!_rom_info.is_nes_20_format)
        return 0;

    
}
std::unique_ptr<Mapper> Cartrigde::makeMapper()
{
    switch (_rom_info.mapper_number) {
    case 0:
        return make_unique<Mapper_000>(&_rom_info, _interrupt_line, _raw_data);
    default:
        return nullptr;
    }
}
void Cartrigde::dumpRomInfo()
{
    _log_module.setBufferID(BufferID::CartrigdeID);
    _log_module << "+++ ROM INFO +++\n"
                << "PRG ROM units: " << _rom_info.PRG_ROM_units << '\n'
                << "chr rom units: " << _rom_info.CHR_ROM_units << '\n'
                << "mapper number: " << _rom_info.mapper_number << '\n'
                << "mirroring: "
                << (_rom_info.mirroring == 1 ? "vertical" : "horizontal")
                << '\n';
}
