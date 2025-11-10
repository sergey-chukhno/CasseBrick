#include "core/states/GameOverState.h"
#include "core/Game.h"
#include "core/states/PlayingState.h"
#include "core/states/MenuState.h"
#include "core/FontManager.h"
#include <iostream>
#include <sstream>
#include <memory>
#include <string>

GameOverState::GameOverState(Game* game, int score)
    : game_(game)
    , score_(score)
    , font_(FontManager::getDefaultFont())
    , titleText_(font_, "GAME OVER", 64)
    , scoreText_(font_, "", 36)  // Initialize with empty string, will set in initializeUI
    , buttonLabels_{"RESTART", "MENU"}
{
    initializeUI();
}

void GameOverState::initializeUI()
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

    // Score text
    std::stringstream ss;
    ss << "FINAL SCORE: " << score_;
    scoreText_.setString(ss.str());
    scoreText_.setFillColor(sf::Color(0, 217, 255)); // Cyan
    scoreText_.setStyle(sf::Text::Bold);
    
    // Center score text
    // SFML 3.0: Rect uses .size (Vector2f) instead of .width/.height
    sf::FloatRect scoreBounds = scoreText_.getLocalBounds();
    scoreText_.setOrigin(sf::Vector2f(scoreBounds.size.x / 2.0f, scoreBounds.size.y / 2.0f));
    scoreText_.setPosition(sf::Vector2f(
        static_cast<float>(game_->getWindowWidth()) / 2.0f,
        300.0f
    ));

    // Create buttons
    float startY = 450.0f;
    float centerX = static_cast<float>(game_->getWindowWidth()) / 2.0f;

    for (size_t i = 0; i < buttonLabels_.size(); ++i)
    {
        // Create button rectangle
        sf::RectangleShape button(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
        button.setFillColor(BUTTON_COLOR);
        button.setOutlineColor(sf::Color(0, 217, 255)); // Cyan outline
        button.setOutlineThickness(2.0f);
        // SFML 3.0: setOrigin and setPosition take Vector2f
        button.setOrigin(sf::Vector2f(BUTTON_WIDTH / 2.0f, BUTTON_HEIGHT / 2.0f));
        button.setPosition(sf::Vector2f(centerX, startY + i * (BUTTON_HEIGHT + BUTTON_SPACING)));
        buttons_.push_back(button);

        // Create button text
        // SFML 3.0: Text requires font in constructor
        sf::Text text(font_, buttonLabels_[i], 24);
        text.setFillColor(TEXT_COLOR);
        text.setStyle(sf::Text::Bold);
        
        // Center text on button
        // SFML 3.0: Rect uses .size (Vector2f) instead of .width/.height
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin(sf::Vector2f(textBounds.size.x / 2.0f, textBounds.size.y / 2.0f));
        text.setPosition(sf::Vector2f(centerX, startY + i * (BUTTON_HEIGHT + BUTTON_SPACING)));
        buttonTexts_.push_back(text);
    }
}

void GameOverState::update(float deltaTime)
{
    // Suppress unused parameter warning
    (void)deltaTime;
    
    // Update button hover states based on mouse position
    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(game_->getWindow());
    sf::Vector2f mousePos = game_->getWindow().mapPixelToCoords(mousePixelPos);
    updateButtonHover(mousePos);
}

void GameOverState::render(sf::RenderWindow& window)
{
    // Draw title
    window.draw(titleText_);

    // Draw score
    window.draw(scoreText_);

    // Draw buttons
    for (size_t i = 0; i < buttons_.size(); ++i)
    {
        window.draw(buttons_[i]);
        window.draw(buttonTexts_[i]);
    }
}

void GameOverState::handleEvent(const sf::Event& event)
{
    if (auto* mouseButton = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouseButton->button == sf::Mouse::Button::Left)
        {
            sf::Vector2i mousePixelPos = sf::Mouse::getPosition(game_->getWindow());
            sf::Vector2f mousePos = game_->getWindow().mapPixelToCoords(mousePixelPos);
            handleButtonClick(mousePos);
        }
    }

    // Keyboard navigation
    if (auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyPressed->code == sf::Keyboard::Key::Enter || keyPressed->code == sf::Keyboard::Key::Space)
        {
            // Activate first button (RESTART)
            handleButtonClick(sf::Vector2f(
                static_cast<float>(game_->getWindowWidth()) / 2.0f,
                450.0f
            ));
        }
    }
}

void GameOverState::onEnter()
{
    std::cout << "Entered GameOverState" << std::endl;
}

void GameOverState::onExit()
{
    std::cout << "Exited GameOverState" << std::endl;
}

void GameOverState::handleButtonClick(const sf::Vector2f& mousePos)
{
    int buttonIndex = getButtonAt(mousePos);
    if (buttonIndex == -1)
    {
        return;
    }

    switch (buttonIndex)
    {
        case 0: // RESTART
            std::cout << "Restart button clicked" << std::endl;
            game_->changeState(std::make_unique<PlayingState>(game_));
            break;

        case 1: // MENU
            std::cout << "Menu button clicked" << std::endl;
            game_->changeState(std::make_unique<MenuState>(game_));
            break;

        default:
            break;
    }
}

void GameOverState::updateButtonHover(const sf::Vector2f& mousePos)
{
    int hoveredButton = getButtonAt(mousePos);
    
    for (size_t i = 0; i < buttons_.size(); ++i)
    {
        if (static_cast<int>(i) == hoveredButton)
        {
            buttons_[i].setFillColor(BUTTON_HOVER_COLOR);
        }
        else
        {
            buttons_[i].setFillColor(BUTTON_COLOR);
        }
    }
}

int GameOverState::getButtonAt(const sf::Vector2f& mousePos) const
{
    for (size_t i = 0; i < buttons_.size(); ++i)
    {
        if (buttons_[i].getGlobalBounds().contains(mousePos))
        {
            return static_cast<int>(i);
        }
    }
    return -1;
}

