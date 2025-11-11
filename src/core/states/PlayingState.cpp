#include "core/states/PlayingState.h"
#include "core/Game.h"
#include "core/states/PausedState.h"
#include "core/states/GameOverState.h"
#include "core/states/MenuState.h"
#include "core/FontManager.h"
#include "entities/Cannon.h"
#include "entities/Projectile.h"
#include "entities/Block.h"
#include <iostream>
#include <memory>
#include <cstdlib>
#include <ctime>

PlayingState::PlayingState(Game* game)
    : game_(game)
    , font_(FontManager::getDefaultFont())
    , placeholderText_(
        font_,
        "PLAYING STATE\n\nPress SPACE or CLICK to shoot\nPress P to Pause\nPress G for Game Over (testing)\nPress ESC to return to Menu",
        32
    )
    , projectilePool_(100) // Pool size: 100 projectiles
{
    // Basic text initialization - positioning will be done in onEnter()
    placeholderText_.setFillColor(sf::Color(0, 217, 255)); // Cyan
    placeholderText_.setStyle(sf::Text::Bold);
}

void PlayingState::update(float deltaTime)
{
    // Update cannon
    if (cannon_)
    {
        cannon_->update(deltaTime, game_->getWindow());
        
        // Update all projectiles
        sf::Vector2u windowSize(game_->getWindowWidth(), game_->getWindowHeight());
        sf::FloatRect cannonBounds = cannon_->getBounds();
        projectilePool_.updateAll(deltaTime, windowSize, cannonBounds);
    }
    
    // Update test blocks (for Step 3.1, temporary)
    for (auto& block : testBlocks_) {
        if (block && !block->isDestroyed()) {
            block->update(deltaTime);
        }
    }
}

void PlayingState::render(sf::RenderWindow& window)
{
    // Render test blocks (behind projectiles and cannon)
    for (const auto& block : testBlocks_) {
        if (block && !block->isDestroyed()) {
            block->render(window);
        }
    }
    
    // Render projectiles (behind cannon)
    projectilePool_.renderAll(window);
    
    // Render cannon
    if (cannon_)
    {
        cannon_->render(window);
    }
    
    // Draw placeholder text (temporary, will be removed later)
    window.draw(placeholderText_);
}

void PlayingState::handleEvent(const sf::Event& event)
{
    // Pass events to cannon for angle control
    if (cannon_)
    {
        cannon_->handleInput(event, game_->getWindow());
    }
    
    // Handle shooting (Space bar or mouse click)
    if (cannon_ && cannon_->canShoot())
    {
        bool shootRequested = false;
        
        if (auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->code == sf::Keyboard::Key::Space)
            {
                shootRequested = true;
            }
        }
        
        if (auto* mouseButton = event.getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouseButton->button == sf::Mouse::Button::Left)
            {
                shootRequested = true;
            }
        }
        
        if (shootRequested)
        {
            // Shoot projectile from cannon
            sf::Vector2f spawnPosition;
            sf::Vector2f velocity;
            if (cannon_->shoot(spawnPosition, velocity))
            {
                // Acquire projectile from pool
                Projectile* projectile = projectilePool_.acquire(spawnPosition, velocity);
                if (!projectile)
                {
                    std::cerr << "Warning: Projectile pool is full!" << std::endl;
                }
            }
        }
    }
    
    // Handle game state transitions
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
            game_->queueStateChange(std::make_unique<GameOverState>(game_, 1234)); // Test score
        }
        // ESC key: Return to menu
        else if (keyPressed->code == sf::Keyboard::Key::Escape)
        {
            std::cout << "Returning to menu" << std::endl;
            game_->queueStateChange(std::make_unique<MenuState>(game_));
        }
    }
}

void PlayingState::onEnter()
{
    std::cout << "Entered PlayingState" << std::endl;
    
    try
    {
        // Initialize cannon at bottom center
        float cannonX = static_cast<float>(game_->getWindowWidth()) / 2.0f;
        float cannonY = static_cast<float>(game_->getWindowHeight()) - 50.0f; // 50px from bottom
        std::cout << "Creating cannon at (" << cannonX << ", " << cannonY << ")" << std::endl;
        
        cannon_ = std::make_unique<Cannon>(
            sf::Vector2f(cannonX, cannonY),
            10 // Initial projectile count
        );
        
        std::cout << "Cannon created successfully" << std::endl;
        
        // Create test blocks for Step 3.1 (temporary, will be replaced by BlockManager in Step 3.2)
        // Initialize random seed for block generation
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        
        int testLevel = 1;
        float windowWidth = static_cast<float>(game_->getWindowWidth());
        
        // Create a few test blocks at different positions
        for (int i = 0; i < 5; ++i) {
            float x = windowWidth / 6.0f * (i + 1); // Spread across screen
            float y = 150.0f + (i * 80.0f); // Staggered vertically
            
            BlockShape shape = Block::getRandomShape(testLevel);
            sf::Color color = Block::getRandomColor();
            
            // New Block constructor: position, shapeType, baseColor, level
            testBlocks_.push_back(std::make_unique<Block>(
                sf::Vector2f(x, y),
                shape,
                color,
                testLevel
            ));
        }
        
        std::cout << "Test blocks created: " << testBlocks_.size() << std::endl;
        
        // Center placeholder text now that we're safely in the state stack
        // SFML 3.0: Rect uses .size (Vector2f) instead of .width/.height
        sf::FloatRect textBounds = placeholderText_.getLocalBounds();
        placeholderText_.setOrigin(sf::Vector2f(textBounds.size.x / 2.0f, textBounds.size.y / 2.0f));
        placeholderText_.setPosition(sf::Vector2f(
            static_cast<float>(game_->getWindowWidth()) / 2.0f,
            100.0f // Position above cannon
        ));
        
        std::cout << "PlayingState initialization complete" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error in PlayingState::onEnter(): " << e.what() << std::endl;
        throw;
    }
    catch (...)
    {
        std::cerr << "Unknown error in PlayingState::onEnter()" << std::endl;
        throw;
    }
}

void PlayingState::onExit()
{
    std::cout << "Exited PlayingState" << std::endl;
}

