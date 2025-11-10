#include "core/Game.h"
#include "core/states/MenuState.h"
#include <iostream>

Game::Game()
    : window_(sf::VideoMode(sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT)), WINDOW_TITLE)
    , running_(true)
{
    window_.setFramerateLimit(60);
    
    // Start with MenuState
    changeState(std::make_unique<MenuState>(this));
    
    std::cout << "Game initialized" << std::endl;
}

Game::~Game()
{
    // Clean up states
    stateStack_.clear();
    std::cout << "Game destroyed" << std::endl;
}

void Game::run()
{
    while (running_ && window_.isOpen())
    {
        // Calculate delta time
        float deltaTime = clock_.restart().asSeconds();

        // Handle window events
        handleWindowEvents();

        // Update current state
        if (!stateStack_.empty())
        {
            update(deltaTime);
            render();
        }
        else
        {
            // No states left, exit
            running_ = false;
        }
    }
}

void Game::pushState(std::unique_ptr<GameState> state)
{
    if (!stateStack_.empty())
    {
        // Notify current state that it's being paused
        stateStack_.back()->onExit();
    }

    // Push new state
    stateStack_.push_back(std::move(state));
    stateStack_.back()->onEnter();
}

void Game::popState()
{
    if (!stateStack_.empty())
    {
        // Notify current state that it's being exited
        stateStack_.back()->onExit();
        stateStack_.pop_back();

        // Notify new current state that it's being resumed
        if (!stateStack_.empty())
        {
            stateStack_.back()->onEnter();
        }
    }
}

void Game::changeState(std::unique_ptr<GameState> state)
{
    // Clear the entire state stack
    while (!stateStack_.empty())
    {
        stateStack_.back()->onExit();
        stateStack_.pop_back();
    }

    // Set new state
    if (state)
    {
        stateStack_.push_back(std::move(state));
        stateStack_.back()->onEnter();
    }
}

GameState* Game::getCurrentState() const
{
    if (stateStack_.empty())
    {
        return nullptr;
    }
    return stateStack_.back().get();
}

void Game::handleWindowEvents()
{
    // Handle global window events (like window close)
    while (std::optional<sf::Event> event = window_.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window_.close();
            running_ = false;
            return;
        }

        // Pass event to current state
        if (!stateStack_.empty())
        {
            stateStack_.back()->handleEvent(*event);
        }
    }
}

void Game::update(float deltaTime)
{
    if (!stateStack_.empty())
    {
        stateStack_.back()->update(deltaTime);
    }
}

void Game::render()
{
    window_.clear(sf::Color(10, 10, 26)); // Cyberpunk background color (#0a0a1a)

    if (!stateStack_.empty())
    {
        stateStack_.back()->render(window_);
    }

    window_.display();
}

