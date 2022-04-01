#pragma once
#include <SFML/Graphics.hpp>
#include <array>

namespace gnes {
class VirtualScreen final : public sf::Drawable {
  public:
    VirtualScreen();
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
    void setPixel(int x, int y, int c);

    static constexpr std::size_t ScreenWidth = 256;
    static constexpr std::size_t ScreenHeight = 240;
  private:

    static const std::array<sf::Color, 0xFF> Pallete;
    std::array<sf::Vertex, ScreenHeight * ScreenWidth> _pixels;
};
} // namespace gnes
