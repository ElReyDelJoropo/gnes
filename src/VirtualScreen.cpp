#include "VirtualScreen.hpp"
#include <array>
using std::array;
#include <SFML/System/Vector2.hpp>
using namespace gnes;


const array<sf::Color,0xFF> VirtualScreen::Pallete = {
    sf::Color(255,255,255),
    sf::Color::Blue,
    sf::Color::Yellow,
    sf::Color::Red,
    sf::Color(48,0,136),
    sf::Color(68,0,100),
    sf::Color(92,0,48),
    sf::Color(84,4,0),
    sf::Color(60,24,0),
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
