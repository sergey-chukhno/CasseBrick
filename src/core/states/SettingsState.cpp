#include "core/states/SettingsState.h"
#include "core/Game.h"
#include "core/states/MenuState.h"
#include "core/FontManager.h"
#include <iostream>
#include <memory>

SettingsState::SettingsState(Game* game, GameState* previousState)
    : game_(game)
    , previousState_(previousState)
    , font_(FontManager::getDefaultFont())
    , titleText_(font_, "SETTINGS", 64)
    , placeholderText_(font_, "Settings menu\n\n(Volume, Controls, etc.)\n\nWill be implemented later", 24)
    , backButtonText_(font_, "BACK", 24)
{
    initializeUI();
}

void SettingsState::initializeUI()
{
    // Title
    titleText_.setFillColor(TITLE_COLOR);
    titleText_.setStyle(sf::Text::Bold);
    
    // Center title
    // SFML 3.0: Rect uses .size (Vector2f) instead of .width/.height
    sf::FloatRect titleBounds = titleText_.getLocalBounds();
    titleText_.setOrigin(sf::Vector2f(titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f));
    titleText_.setPosition(sf::Vector2f(
        static_cast<float>(game_->getWindowWidth()) / 2.0f,
        200.0f
    ));

    // Placeholder text - position it higher to avoid overlapping with buttons
    placeholderText_.setFillColor(sf::Color(0, 217, 255)); // Cyan
    placeholderText_.setStyle(sf::Text::Bold);
    
    // Center placeholder text - position it between title and back button
    // SFML 3.0: Rect uses .size (Vector2f) instead of .width/.height
    sf::FloatRect placeholderBounds = placeholderText_.getLocalBounds();
    placeholderText_.setOrigin(sf::Vector2f(placeholderBounds.size.x / 2.0f, placeholderBounds.size.y / 2.0f));
    placeholderText_.setPosition(sf::Vector2f(
        static_cast<float>(game_->getWindowWidth()) / 2.0f,
        350.0f  // Position higher, between title (200) and back button (600)
    ));

    // Back button
    float centerX = static_cast<float>(game_->getWindowWidth()) / 2.0f;
    float buttonY = 550.0f;  // Move back button up a bit to give more space

    backButton_.setSize(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
    backButton_.setFillColor(BUTTON_COLOR);
    backButton_.setOutlineColor(sf::Color(0, 217, 255)); // Cyan outline
    backButton_.setOutlineThickness(2.0f);
    // SFML 3.0: setOrigin and setPosition take Vector2f
    backButton_.setOrigin(sf::Vector2f(BUTTON_WIDTH / 2.0f, BUTTON_HEIGHT / 2.0f));
    backButton_.setPosition(sf::Vector2f(centerX, buttonY));

    backButtonText_.setFillColor(TEXT_COLOR);
    backButtonText_.setStyle(sf::Text::Bold);
    
    // Center text on button
    // SFML 3.0: Rect uses .size (Vector2f) instead of .width/.height
    sf::FloatRect textBounds = backButtonText_.getLocalBounds();
    backButtonText_.setOrigin(sf::Vector2f(textBounds.size.x / 2.0f, textBounds.size.y / 2.0f));
    backButtonText_.setPosition(sf::Vector2f(centerX, buttonY));
}

void SettingsState::update(float deltaTime)
{
    // Suppress unused parameter warning
    (void)deltaTime;
    
    // Update back button hover state
    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(game_->getWindow());
    sf::Vector2f mousePos = game_->getWindow().mapPixelToCoords(mousePixelPos);
    
    if (backButton_.getGlobalBounds().contains(mousePos))
    {
        backButton_.setFillColor(BUTTON_HOVER_COLOR);
    }
    else
    {
        backButton_.setFillColor(BUTTON_COLOR);
    }
}

void SettingsState::render(sf::RenderWindow& window)
{
    // Note: Window is cleared by Game::render() before this is called
    // SettingsState only renders its own UI elements
    
    // Draw title (SETTINGS only, not the main game title)
    window.draw(titleText_);

    // Draw placeholder text
    window.draw(placeholderText_);

    // Draw back button
    window.draw(backButton_);
    window.draw(backButtonText_);
}

void SettingsState::handleEvent(const sf::Event& event)
{
    if (auto* mouseButton = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouseButton->button == sf::Mouse::Button::Left)
        {
            sf::Vector2i mousePixelPos = sf::Mouse::getPosition(game_->getWindow());
            sf::Vector2f mousePos = game_->getWindow().mapPixelToCoords(mousePixelPos);
            
            if (backButton_.getGlobalBounds().contains(mousePos))
            {
                handleBackButton();
            }
        }
    }

    // Keyboard navigation
    if (auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        // ESC or Backspace: Go back
        if (keyPressed->code == sf::Keyboard::Key::Escape || keyPressed->code == sf::Keyboard::Key::Backspace)
        {
            handleBackButton();
        }
    }
}

void SettingsState::onEnter()
{
    std::cout << "Entered SettingsState" << std::endl;
}

void SettingsState::onExit()
{
    std::cout << "Exited SettingsState" << std::endl;
}

void SettingsState::handleBackButton()
{
    std::cout << "Back button clicked" << std::endl;
    // Return to menu (Settings uses changeState, so we need to create a new MenuState)
    game_->changeState(std::make_unique<MenuState>(game_));
}

