#include "VirtualScreen.hpp"
#include <array>
using std::array;
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
using namespace gnes;


const array<sf::Color,0xFF> VirtualScreen::Pallete = {
    sf::Color(84,84,84),
    sf::Color(0,30,16),
    sf::Color(8,16,44),
    sf::Color(48,0,136),
    sf::Color(68,0,100),
    sf::Color(92,0,48),
    sf::Color(84,4,0),
    sf::Color(60,24,0),
    sf::Color(32,42,0),
    sf::Color(8,58,0),
    sf::Color(0,64,0),
    sf::Color(0,60,0),
    sf::Color(0,50,60),
    sf::Color(0,0,0),

    sf::Color(152,150,152),
    sf::Color(8,76,196),
    sf::Color(48,50,236),
    sf::Color(92,30,228),
    sf::Color(136,20,176),
    sf::Color(160,20,100),
    sf::Color(152,34,32),
    sf::Color(120,60,0),
    sf::Color(84,90,0),
    sf::Color(40,114,0),
    sf::Color(8,124,0),
    sf::Color(0,118,40),
    sf::Color(0,102,120),
    sf::Color(0,0,0),

    sf::Color(0,30,16),
    sf::Color(0,30,16),
    sf::Color(0,30,16),
    sf::Color::Blue,
    sf::Color::Yellow,
    sf::Color::Red,
    sf::Color(48,0,136),
    sf::Color(68,0,100),
    sf::Color(92,0,48),
    sf::Color(84,4,0),
    sf::Color(60,24,0),
    sf::Color(84,84,84),
    sf::Color(84,84,84),
};

VirtualScreen::VirtualScreen()
{
    auto sz = _pixels.size();
    for (decltype(sz) i = 0; i != sz; ++i){
        _pixels.at(i).position = sf::Vector2f(i % 256, i / 256);
        _pixels.at(i).color = sf::Color::Black;
    }
}
void VirtualScreen::setPixel(int x, int y, int c)
{
    _pixels.at(ScreenWidth * y + x).color = Pallete.at(c);
}
void VirtualScreen::draw(sf::RenderTarget &target,
                         sf::RenderStates states) const
{
    target.draw(_pixels.data(), _pixels.size(), sf::PrimitiveType::Points,
                states);
}
