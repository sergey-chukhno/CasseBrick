#include "core/FontManager.h"
#include <iostream>

std::unique_ptr<sf::Font> FontManager::defaultFont_ = nullptr;
bool FontManager::fontLoaded_ = false;

const sf::Font &FontManager::getDefaultFont() {
  if (!fontLoaded_) {
    defaultFont_ = std::make_unique<sf::Font>();

    // Try to load a system font
    // Common system font paths on macOS
    const std::vector<std::string> systemFontPaths = {
        "/System/Library/Fonts/Helvetica.ttc",
        "/System/Library/Fonts/SFNSDisplay.ttf",
        "/Library/Fonts/Arial.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf",
    };

    bool loaded = false;
    for (const auto &path : systemFontPaths) {
      if (defaultFont_->openFromFile(path)) {
        loaded = true;
        std::cout << "Loaded font from: " << path << std::endl;
        break;
      }
    }

    if (!loaded) {
      // If no system font found, we'll use SFML's default font handling
      // For now, we'll create an empty font and handle it gracefully
      std::cerr << "Warning: Could not load system font. Text may not render "
                   "correctly."
                << std::endl;
      // Note: In SFML 3.0, we might need to handle this differently
      // For now, we'll proceed and handle font loading errors in the states
    }

    fontLoaded_ = true;
  }

  return *defaultFont_;
}

bool FontManager::loadSystemFont() {
  // This is called from getDefaultFont()
  return fontLoaded_;
}

void FontManager::cleanup() {
  if (fontLoaded_) {
    defaultFont_.reset();
    fontLoaded_ = false;
  }
}
