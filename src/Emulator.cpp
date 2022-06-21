#include "Emulator.hpp"

#include <filesystem>
#include <SFML/Graphics.hpp>
using std::filesystem::path;

using namespace gnes;

Emulator::Emulator(): _cpu_bus(&_cartrigde), _cpu(&_cpu_bus,&_interrupt_line,&_log_module), _ppu(&_virtual_screen,&_cartrigde), _cartrigde(&_interrupt_line,&_log_module){}



void Emulator::run(path p){
    _cartrigde.load(p);
    _cpu.powerUp();
    _cpu.reset();
    _ppu.reset();
    //_ppu.dumpPatternTable();

    sf::RenderWindow w(sf::VideoMode(VirtualScreen::ScreenWidth,VirtualScreen::ScreenHeight),"Gnes");
    //w.setFramerateLimit(60);

    while(w.isOpen()){
        sf::Event e;
        while(w.pollEvent(e)){
            if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape)
                w.close();
        }
        tick();
        w.draw(_virtual_screen); 
        w.display();
        w.clear();
    }

    //for (int i = 0; i != 100; ++i){
    //    tick();
    //}
}

void Emulator::tick(){
    _cpu.step();
    _ppu.step();
    _ppu.step();
    _ppu.step();
}
