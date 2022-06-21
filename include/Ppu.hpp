#pragma once
#include "Cartrigde.hpp"
#include "InterruptLine.hpp"
#include "PpuBus.hpp"
#include "VirtualScreen.hpp"
#include "types.hpp"

namespace gnes {
enum PpuRegister {
    PpuCtrl = 0x2000,
    PpuMask,
    PpuStatus,
    OamAddr,
    OamData,
    PpuScroll,
    PpuAddr,
    PpuData,
    OamDma
};
enum PpuRenderingState { PreRender, Render, PostRender, VerticalBlank };

class Ppu {
  public:
      Ppu(VirtualScreen *vs, Cartrigde *c);
    void step();
    void reset();
    uByte read(std::uint16_t address);
    void write(std::uint16_t address, uByte b);

  private:
    PpuBus _bus;
    VirtualScreen *_virtual_screen;
    int _cycles = 0;
    int _scanline = 240;
    bool _even = false;

    union {
        struct {
            uByte base_nametable_address : 2;
            uByte vram_increment : 1;
            uByte sprite_chr_bank : 1;
            uByte background_chr_bank : 1;
            uByte sprite_size : 1;
            uByte master : 1;
            uByte raise_nmi : 1;
        };
        uByte data = 0;
    } _ppu_ctrl;
    union {
        struct {
            uByte greyscale : 1;
            uByte show_background_leftmost : 1;
            uByte show_sprites_leftmost : 1;
            uByte show_background : 1;
            uByte show_sprites : 1;
            uByte emphasize_red : 1;
            uByte emphasize_green : 1;
            uByte emphasize_blue : 1;
        };
        uByte data = 0;
    } _ppu_mask;
    union {
        struct {

            uByte unused : 5;
            uByte sprite_overflow : 1;
            uByte sprite_0_hit : 1;
            uByte vertical_black : 1;
        };
        uByte data = 0;
    } _ppu_status;
    uByte _oam_addr = 0;
    uByte _oam_data = 0;
    uByte _ppu_scroll = 0;
    union{
        struct{
            uByte coarse_x:5;
            uByte coarse_y:5;
            uByte nametable_select:2;
            uByte fine_y:3;
            uByte pad:1;
        };
        std::uint16_t data = 0;
    } _v, _t;
    int _address_latch = 0;
    uByte fine_x = 0;
    uByte _ppu_data = 0;
    uByte _oam_dma = 0;

    void preRender();
    void render();
    void postRender();

    void incrementX();
    void incrementY();

    public:
    void dumpPatternTable();
};
} // namespace gnes
