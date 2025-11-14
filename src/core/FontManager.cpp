#include "core/FontManager.h"
#include <iostream>
#include <filesystem>

std::unique_ptr<sf::Font> FontManager::defaultFont_ = nullptr;
bool FontManager::fontLoaded_ = false;

const sf::Font& FontManager::getDefaultFont()
{
    if (!fontLoaded_)
    {
        defaultFont_ = std::make_unique<sf::Font>();

        bool loaded = false;

        // 1️⃣ // Try to load the local font
        const std::string localFontPath = "fonts/OpenSans-Regular.ttf";
        if (std::filesystem::exists(localFontPath) && defaultFont_->openFromFile(localFontPath))
        {
            loaded = true;
            std::cout << "Loaded font from local project: " << localFontPath << std::endl;
        }

        // 2️⃣ Otherwise load system fonts based on the OS
        if (!loaded)
        {
#ifdef _WIN32
            const std::vector<std::string> systemFontPaths = {
                "C:/Windows/Fonts/Arial.ttf",
                "C:/Windows/Fonts/SegoeUI.ttf"
            };
#elif __APPLE__
            const std::vector<std::string> systemFontPaths = {
                "/System/Library/Fonts/Helvetica.ttc",
                "/System/Library/Fonts/SFNSDisplay.ttf",
                "/Library/Fonts/Arial.ttf",
                "/System/Library/Fonts/Supplemental/Arial.ttf"
            };
#else
            const std::vector<std::string> systemFontPaths = {
                "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                "/usr/share/fonts/truetype/freefont/FreeSans.ttf"
            };
#endif
            for (const auto& path : systemFontPaths)
            {
                if (std::filesystem::exists(path) && defaultFont_->openFromFile(path))
                {
                    loaded = true;
                    std::cout << "Loaded system font from: " << path << std::endl;
                    break;
                }
            }
        }

        // 3️⃣ If no font found
        if (!loaded)
        {
            std::cerr << "⚠️ Could not load any font. Text may not render correctly." << std::endl;
        }

        fontLoaded_ = true;
    }

    return *defaultFont_;
}

bool FontManager::loadSystemFont()
{
    return fontLoaded_;
}
