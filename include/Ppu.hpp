#pragma once
#include "Cartrigde.hpp"
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
    uByte read(std::uint16_t address);
    void write(std::uint16_t address, uByte b);

  private:
    PpuBus _bus;
    VirtualScreen *_virtual_screen;
    PpuRenderingState _state;
    int _cycles;
    int _scanline;
    bool _even;

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
        uByte data;
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
        uByte data;
    } _ppu_mask;
    union {
        struct {

            uByte unused : 5;
            uByte sprite_overflow : 1;
            uByte sprite_0_hit : 1;
            uByte vertical_black : 1;
        };
        uByte data;
    } _ppu_status;
    uByte _oam_addr;
    uByte _oam_data;
    uByte _ppu_scroll;
    struct{
        union{
            uByte coarse_x:5;
            uByte coarse_y:5;
            uByte nametable_select:2;
            uByte fine_y:3;
            uByte pad:1;
        };
        std::uint16_t data;
    } _v, _t;
    int _address_latch;
    uByte fine_x;
    uByte _ppu_data;
    uByte _oam_dma;

    void preRender();
    void render();
    void postRender();
    void reset();

    public:
    void dumpPatternTable();
};
} // namespace gnes
