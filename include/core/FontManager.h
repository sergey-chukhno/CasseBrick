#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

/**
 * @brief Simple font manager for loading and managing fonts.
 *
 * FontManager provides a static method to get a default font.
 * For now, it tries to load a system font or creates a fallback.
 * In the future, this will be expanded to load fonts from assets.
 */
class FontManager {
public:
  /**
   * @brief Gets the default font.
   *
   * Tries to load a system font. If that fails, returns a fallback font.
   *
   * @return Reference to the default font
   */
  static const sf::Font &getDefaultFont();

private:
  static std::unique_ptr<sf::Font> defaultFont_;
  static bool fontLoaded_;

  /**
   * @brief Attempts to load a system font.
   * @return True if font was loaded successfully, false otherwise
   */
  static bool loadSystemFont();

public:
  /**
   * @brief Cleans up the default font.
   * Should be called before the program exits.
   */
  static void cleanup();
};

#endif // FONTMANAGER_H
