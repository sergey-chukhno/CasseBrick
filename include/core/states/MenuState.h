#ifndef MENUSTATE_H
#define MENUSTATE_H

#include "core/GameState.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>

// Forward declarations
class Game;
class Button;
class Starfield;
class AnimatedText;

/**
 * @brief Menu state with Start, Settings, and Quit buttons.
 * 
 * MenuState displays the main menu with buttons that allow the player to:
 * - Start the game (transitions to PlayingState)
 * - Open settings (transitions to SettingsState)
 * - Quit the game (exits application)
 * 
 * Enhanced with cyberpunk aesthetics:
 * - Animated starfield background
 * - Glowing buttons with hover effects
 * - Pulsing title text
 */
class MenuState : public GameState
{
public:
    /**
     * @brief Constructs a MenuState.
     * @param game Pointer to the Game instance for state transitions
     */
    explicit MenuState(Game* game);

    /**
     * @brief Destructor.
     */
    ~MenuState() override;

    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event) override;
    void onEnter() override;
    void onExit() override;

private:
    Game* game_;

    // UI components
    std::unique_ptr<Starfield> starfield_;
    std::unique_ptr<AnimatedText> titleText_;
    std::vector<std::unique_ptr<Button>> buttons_;
    std::vector<std::string> buttonLabels_;

    // Button dimensions
    static constexpr float BUTTON_WIDTH = 300.0f;
    static constexpr float BUTTON_HEIGHT = 60.0f;
    static constexpr float BUTTON_SPACING = 20.0f;

    // Colors
    static constexpr sf::Color TITLE_COLOR = sf::Color(255, 0, 110);  // Pink #ff006e
    static constexpr sf::Color BUTTON_FILL_COLOR = sf::Color(0, 217, 255, 100);  // Cyan with transparency
    static constexpr sf::Color BUTTON_OUTLINE_COLOR = sf::Color(0, 217, 255);  // Cyan
    static constexpr sf::Color BUTTON_TEXT_COLOR = sf::Color(255, 255, 255);  // White

    /**
     * @brief Initializes the UI elements.
     */
    void initializeUI();
};

#endif // MENUSTATE_H
