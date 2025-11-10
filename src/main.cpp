#include <SFML/Graphics.hpp>
#include <iostream>
#include <optional>
#include <string>

/**
 * @brief Main entry point for the Cyberpunk Cannon Shooter game.
 * 
 * Creates the main window and runs the game loop.
 */
int main()
{
    // Window constants
    const unsigned int WINDOW_WIDTH = 1280;
    const unsigned int WINDOW_HEIGHT = 720;
    const std::string WINDOW_TITLE = "Cyberpunk Cannon Shooter";
    
    // Cyberpunk background color (#0a0a1a)
    const sf::Color BACKGROUND_COLOR(10, 10, 26);
    
    // Create the main window
    // SFML 3.0: VideoMode now takes Vector2u instead of two unsigned ints
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT)), WINDOW_TITLE);
    window.setFramerateLimit(60); // Limit to 60 FPS
    
    std::cout << "Cyberpunk Cannon Shooter - Game Started" << std::endl;
    std::cout << "Press ESC to exit" << std::endl;
    
    // Main game loop
    while (window.isOpen())
    {
        // Handle events
        // SFML 3.0: pollEvent() returns std::optional<Event> instead of taking Event by reference
        while (std::optional<sf::Event> event = window.pollEvent())
        {
            // SFML 3.0: Events are now accessed via std::visit or pattern matching
            // Check if window should close
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            
            // Exit on ESC key
            if (auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                // SFML 3.0: Key codes are now scoped enums
                if (keyPressed->code == sf::Keyboard::Key::Escape)
                {
                    window.close();
                }
            }
        }
        
        // Update game logic (placeholder for now)
        // TODO: Add game update logic
        
        // Clear the window with background color
        window.clear(BACKGROUND_COLOR);
        
        // Draw game objects (placeholder for now)
        // TODO: Add game rendering logic
        
        // Display the window
        window.display();
    }
    
    std::cout << "Game closed" << std::endl;
    return 0;
}

