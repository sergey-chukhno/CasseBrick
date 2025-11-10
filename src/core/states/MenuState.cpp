#include "core/states/MenuState.h"
#include "core/Game.h"
#include "core/states/PlayingState.h"
#include "core/states/SettingsState.h"
#include "core/FontManager.h"
#include "ui/Button.h"
#include "ui/Starfield.h"
#include "ui/AnimatedText.h"
#include <iostream>
#include <memory>

MenuState::MenuState(Game* game)
    : game_(game)
    , buttonLabels_{"START", "SETTINGS", "QUIT"}
{
    initializeUI();
}

MenuState::~MenuState() = default;

void MenuState::initializeUI()
{
    const sf::Font& font = FontManager::getDefaultFont();
    
    // Initialize starfield
    starfield_ = std::make_unique<Starfield>(
        200,
        sf::Vector2u(game_->getWindowWidth(), game_->getWindowHeight())
    );

    // Initialize title with animated text
    titleText_ = std::make_unique<AnimatedText>(
        font,
        "CYBERPUNK CANNON SHOOTER",
        64
    );
    titleText_->setFillColor(TITLE_COLOR);
    titleText_->setPulseRange(0.8f, 1.0f);
    titleText_->setPulseSpeed(1.0f);
    titleText_->setGlowEnabled(true);
    titleText_->setShadowEnabled(true);
    
    // Center title
    sf::FloatRect titleBounds = titleText_->getLocalBounds();
    titleText_->setOrigin(sf::Vector2f(titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f));
    titleText_->setPosition(sf::Vector2f(
        static_cast<float>(game_->getWindowWidth()) / 2.0f,
        150.0f
    ));

    // Create buttons
    float startY = 350.0f;
    float centerX = static_cast<float>(game_->getWindowWidth()) / 2.0f;

    for (size_t i = 0; i < buttonLabels_.size(); ++i)
    {
        float buttonY = startY + i * (BUTTON_HEIGHT + BUTTON_SPACING);
        
        auto button = std::make_unique<Button>(
            font,
            buttonLabels_[i],
            sf::Vector2f(centerX, buttonY),
            sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT)
        );
        
        button->setColors(BUTTON_FILL_COLOR, BUTTON_OUTLINE_COLOR, BUTTON_TEXT_COLOR);

        // Set click callbacks
        // Use queueStateChange to safely defer state changes until after event handling
        if (i == 0)  // START
        {
            button->setOnClick([this]() {
                std::cout << "Start button clicked" << std::endl;
                game_->queueStateChange(std::make_unique<PlayingState>(game_));
            });
        }
        else if (i == 1)  // SETTINGS
        {
            button->setOnClick([this]() {
                std::cout << "Settings button clicked" << std::endl;
                game_->queueStateChange(std::make_unique<SettingsState>(game_));
            });
        }
        else if (i == 2)  // QUIT
        {
            button->setOnClick([this]() {
                std::cout << "Quit button clicked" << std::endl;
                game_->getWindow().close();
            });
        }

        buttons_.push_back(std::move(button));
    }
}

void MenuState::update(float deltaTime)
{
    // Update starfield
    if (starfield_)
    {
        starfield_->update(deltaTime);
    }

    // Update title animation
    if (titleText_)
    {
        titleText_->update(deltaTime);
    }

    // Update buttons
    for (auto& button : buttons_)
    {
        button->update(deltaTime);
    }
}

void MenuState::render(sf::RenderWindow& window)
{
    // Render starfield first (background)
    if (starfield_)
    {
        starfield_->render(window);
    }

    // Render title
    if (titleText_)
    {
        titleText_->render(window);
    }

    // Render buttons
    for (const auto& button : buttons_)
    {
        button->render(window);
    }
}

void MenuState::handleEvent(const sf::Event& event)
{
    // Handle keyboard navigation
    if (auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        if (keyPressed->code == sf::Keyboard::Key::Enter || 
            keyPressed->code == sf::Keyboard::Key::Space)
        {
            // Activate first button (START) - simulate click by triggering state change
            game_->queueStateChange(std::make_unique<PlayingState>(game_));
        }
        else if (keyPressed->code == sf::Keyboard::Key::Escape)
        {
            // Quit game
            game_->getWindow().close();
        }
    }

    // Pass events to buttons (they handle mouse clicks)
    for (auto& button : buttons_)
    {
        button->handleEvent(event, game_->getWindow());
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
