#include "core/states/PlayingState.h"
#include "core/Game.h"
#include "core/states/PausedState.h"
#include "core/states/GameOverState.h"
#include "core/states/MenuState.h"
#include "core/FontManager.h"
#include <iostream>
#include <memory>

PlayingState::PlayingState(Game* game)
    : game_(game)
    , font_(FontManager::getDefaultFont())
    , placeholderText_(
        font_,
        "PLAYING STATE\n\nPress P to Pause\nPress G for Game Over (testing)\nPress ESC to return to Menu",
        32
    )
{
    // Initialize placeholder text
    placeholderText_.setFillColor(sf::Color(0, 217, 255)); // Cyan
    placeholderText_.setStyle(sf::Text::Bold);
    
    // Center text
    // SFML 3.0: Rect uses .size (Vector2f) instead of .width/.height
    sf::FloatRect textBounds = placeholderText_.getLocalBounds();
    placeholderText_.setOrigin(sf::Vector2f(textBounds.size.x / 2.0f, textBounds.size.y / 2.0f));
    placeholderText_.setPosition(sf::Vector2f(
        static_cast<float>(game_->getWindowWidth()) / 2.0f,
        static_cast<float>(game_->getWindowHeight()) / 2.0f
    ));
}

void PlayingState::update(float deltaTime)
{
    // Suppress unused parameter warning
    (void)deltaTime;
    // Placeholder: Update game logic will be implemented later
    // For now, nothing to update
}

void PlayingState::render(sf::RenderWindow& window)
{
    // Draw placeholder text
    window.draw(placeholderText_);
}

void PlayingState::handleEvent(const sf::Event& event)
{
    if (auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        // P key: Pause
        if (keyPressed->code == sf::Keyboard::Key::P)
        {
            std::cout << "Pause key pressed" << std::endl;
            game_->pushState(std::make_unique<PausedState>(game_));
        }
        // G key: Game Over (for testing)
        else if (keyPressed->code == sf::Keyboard::Key::G)
        {
            std::cout << "Game Over triggered (testing)" << std::endl;
            game_->changeState(std::make_unique<GameOverState>(game_, 1234)); // Test score
        }
        // ESC key: Return to menu
        else if (keyPressed->code == sf::Keyboard::Key::Escape)
        {
            std::cout << "Returning to menu" << std::endl;
            game_->changeState(std::make_unique<MenuState>(game_));
        }
    }
}

void PlayingState::onEnter()
{
    std::cout << "Entered PlayingState" << std::endl;
}

void PlayingState::onExit()
{
    std::cout << "Exited PlayingState" << std::endl;
}

