#ifndef SETTINGSSTATE_H
#define SETTINGSSTATE_H

#include "core/GameState.h"
#include <SFML/Graphics.hpp>
#include <memory>

// Forward declaration
class Game;
class GameState;

/**
 * @brief Settings state for game configuration.
 * 
 * SettingsState is displayed when the player wants to change game settings.
 * The back button returns to the menu by creating a new MenuState.
 */
class SettingsState : public GameState
{
public:
    /**
     * @brief Constructs a SettingsState.
     * @param game Pointer to the Game instance for state transitions
     */
    explicit SettingsState(Game* game);

    /**
     * @brief Destructor.
     */
    ~SettingsState() override = default;

    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event) override;
    void onEnter() override;
    void onExit() override;

private:
    Game* game_;

    // UI elements
    const sf::Font& font_;  // Reference to font from FontManager
    sf::Text titleText_;
    sf::Text placeholderText_;
    sf::RectangleShape backButton_;
    sf::Text backButtonText_;

    // Button colors
    static constexpr sf::Color BUTTON_COLOR = sf::Color(0, 217, 255, 150);      // Cyan with transparency
    static constexpr sf::Color BUTTON_HOVER_COLOR = sf::Color(0, 217, 255, 200); // Cyan brighter
    static constexpr sf::Color TEXT_COLOR = sf::Color(255, 255, 255);            // White
    static constexpr sf::Color TITLE_COLOR = sf::Color(255, 0, 110);             // Pink

    // Button dimensions
    static constexpr float BUTTON_WIDTH = 200.0f;
    static constexpr float BUTTON_HEIGHT = 50.0f;

    /**
     * @brief Initializes the UI elements.
     */
    void initializeUI();

    /**
     * @brief Handles back button click.
     */
    void handleBackButton();
};

#endif // SETTINGSSTATE_H

