#include "Ppu.hpp"
#include "PpuBus.hpp"
#include "VirtualScreen.hpp"

using namespace gnes;

Ppu::Ppu(VirtualScreen *vs,Cartrigde *c) : _bus(c),_virtual_screen(vs){}
uByte Ppu::read(uint16_t address)
{
    uByte ret = 0;

    switch (address) {
    case PpuStatus:
        _address_latch = 0;
        ret = _ppu_status.data;
        break;
    case PpuData:
        ret = _bus.read(_v.data);
        _v.data += _ppu_ctrl.vram_increment ? 32 : 1;
        break;
    default:
        break;
    }

    return ret;
}
void Ppu::write(uint16_t address, uByte b)
{
    switch (address) {
    case PpuCtrl:
        _t.nametable_select = b; // t: ...BA.. ........ = d: ......BA
        _ppu_ctrl.data = b;
        break;
    case PpuMask:
        _ppu_mask.data = b;
        break;
    case PpuScroll:
        if (_address_latch == 0) {
            _t.coarse_x = b >> 3; // t: ....... ...HGFED = d: HGFED...
            fine_x = b & 0b111;   // x: CBA = d: .....CBA
        } else {
            _t.fine_y = b & 0b111;
            _t.coarse_y = b >> 3;
        }
        _address_latch ^= 1;
        break;
    case PpuAddr:
        if (_address_latch == 0) {
            _t.data = (b & 0x3F) << 8; // t: .FEDCBA ........ = d: ..FEDCBA
            _t.data &= 0xBFFF;         // t: X....... ........ = 0
        } else {
            _t.data |= b;      // t: ........ HGFEDBA = d: HGFEDBA
            _v.data = _t.data; // v = t
        }
        _address_latch ^= 1;
    }
}

void Ppu::step()
{
    switch (_scanline) {
    case -1:
    case 261:
        preRender();
        break;
    case 0 ... 239:
        render();
        break;
    case 240:
        postRender();
        break;
    case 241:
        _ppu_status.vertical_black = 1;
        break;
    default:
        break;
    }
    if (++_cycles == 341) {
        _cycles = 0;
        ++_scanline;
    }
    if (_scanline == 261) {
        _scanline = -1;
        _even = !_even;
    }
}

void Ppu::preRender() {
    //Fill shifts registers to render
}
void Ppu::postRender() {}
void Ppu::render()
{
    uint16_t bg_addr = 0;
    uByte tile_low = 0;
    uByte tile_high = 0;
    uByte palette = 0;

    switch (_cycles % 8) {
    case 1:
        //Fetch nametable byte
        bg_addr = _bus.read(0x2000 | (_v.data & 0xFFF));
        break;
    case 3:
        //Fetch attribute byte
        palette = _bus.read(bg_addr & 0x2C30);
        break;
    case 5:
        //Fetch tile byte low
        tile_low = _bus.read(bg_addr);
        break;
    case 7:
        //Fetch tile byte high
        tile_high = _bus.read(bg_addr + 8);
    default:
        break;
    }
    //tile = _bus.read(bg_addr);
    //pixel = _bus.read(page * 0x1000 + tile * 16 + fine_y)
                //pallete = (tile_high >> (7 - fine_x) & 0x1) << 1 | (tile_low >> (7 - fine_x) & 0x1);
    incrementX();
}

void Ppu::dumpPatternTable()
{
    uByte tile_high = 0;
    uByte tile_low = 0;
    uByte palette = 0;
    uByte color = 0;
    for (int k = 0; k != 32; ++k)
    for (int i = 0; i != 8; ++i) {
        tile_low = _bus.read(i+16*k);
        tile_high = _bus.read(i + 16*k+ 8);
        for (int j = 0; j != 8; ++j) {
            palette =
                (tile_high >> (7 - j) & 0x1) << 1 | (tile_low >> (7 - j) & 0x1);
            switch (palette) {
            case 0:
                color = 0x00;
                break;
            case 1:
                color = 0x01;
                break;
            case 2:
                color = 0x02;
                break;
            case 3:
                color = 0x3;
                break;
            }
            _virtual_screen->setPixel(j+8*k,i,color);
        }
    }
}
void Ppu::incrementX(){
    if (fine_x != 7)
        return;

    if (_v.coarse_x == 31){
        _v.data ^= 0x0400;
        _v.coarse_x = 0;
    }
    else
        ++_v.data;
}
void Ppu::incrementY(){
    if (_v.fine_y != 7){
        ++_v.fine_y;
    }
    else{
        _v.fine_y = 0;
        if (_v.coarse_y == 29){
            _v.coarse_y = 0;
            _v.data ^= 0x0800;
        }
    }
}
