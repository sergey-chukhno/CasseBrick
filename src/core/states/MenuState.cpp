#include "core/states/MenuState.h"
#include "core/Game.h"
#include "core/states/PlayingState.h"
#include "core/states/SettingsState.h"
#include "core/FontManager.h"
#include <iostream>
#include <memory>

MenuState::MenuState(Game* game)
    : game_(game)
    , font_(FontManager::getDefaultFont())
    , titleText_(font_, "CYBERPUNK CANNON SHOOTER", 48)
    , buttonLabels_{"START", "SETTINGS", "QUIT"}
{
    initializeUI();
}

void MenuState::initializeUI()
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
        150.0f
    ));

    // Create buttons
    float startY = 350.0f;
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

void MenuState::update(float deltaTime)
{
    // Suppress unused parameter warning
    (void)deltaTime;
    
    // Update button hover states based on mouse position
    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(game_->getWindow());
    sf::Vector2f mousePos = game_->getWindow().mapPixelToCoords(mousePixelPos);
    updateButtonHover(mousePos);
}

void MenuState::render(sf::RenderWindow& window)
{
    // Draw title
    window.draw(titleText_);

    // Draw buttons
    for (size_t i = 0; i < buttons_.size(); ++i)
    {
        window.draw(buttons_[i]);
        window.draw(buttonTexts_[i]);
    }
}

void MenuState::handleEvent(const sf::Event& event)
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
            // Activate first button (START)
            handleButtonClick(sf::Vector2f(
                static_cast<float>(game_->getWindowWidth()) / 2.0f,
                350.0f
            ));
        }
        else if (keyPressed->code == sf::Keyboard::Key::Escape)
        {
            // Quit game
            game_->getWindow().close();
        }
    }
}

void MenuState::onEnter()
{
    std::cout << "Entered MenuState" << std::endl;
}

void MenuState::onExit()
{
    std::cout << "Exited MenuState" << std::endl;
}

void MenuState::handleButtonClick(const sf::Vector2f& mousePos)
{
    int buttonIndex = getButtonAt(mousePos);
    if (buttonIndex == -1)
    {
        return;
    }

    switch (buttonIndex)
    {
        case 0: // START
            std::cout << "Start button clicked" << std::endl;
            game_->changeState(std::make_unique<PlayingState>(game_));
            break;

        case 1: // SETTINGS
            std::cout << "Settings button clicked" << std::endl;
            // Use changeState instead of pushState since Settings is a full-screen replacement
            game_->changeState(std::make_unique<SettingsState>(game_, this));
            break;

        case 2: // QUIT
            std::cout << "Quit button clicked" << std::endl;
            game_->getWindow().close();
            break;

        default:
            break;
    }
}

void MenuState::updateButtonHover(const sf::Vector2f& mousePos)
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

int MenuState::getButtonAt(const sf::Vector2f& mousePos) const
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

