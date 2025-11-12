#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

class FontManager
{
public:
    static const sf::Font& getDefaultFont();

private:
    static std::unique_ptr<sf::Font> defaultFont_;
    static bool fontLoaded_;

    static bool loadSystemFont();
};

#endif // FONTMANAGER_H
