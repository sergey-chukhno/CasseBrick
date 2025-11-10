#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "core/GameState.h"

// Forward declaration
class GameState;

/**
 * @brief Main game class that manages the window and game states.
 * 
 * The Game class is responsible for:
 * - Creating and managing the SFML window
 * - Managing the game state stack
 * - Running the main game loop
 * - Handling state transitions (push, pop, change)
 * - Calculating delta time for frame-independent updates
 */
class Game
{
public:
    /**
     * @brief Constructs a Game instance.
     * 
     * Initializes the window with default settings and prepares
     * the game state stack.
     */
    Game();

    /**
     * @brief Destructor.
     */
    ~Game();

    /**
     * @brief Starts the main game loop.
     * 
     * This method runs until the game is closed. It handles events,
     * updates the current state, and renders the current state.
     */
    void run();

    /**
     * @brief Pushes a new state onto the state stack.
     * 
     * This is used for overlay states like Pause, which should
     * return to the previous state when closed.
     * 
     * @param state Unique pointer to the new state
     */
    void pushState(std::unique_ptr<GameState> state);

    /**
     * @brief Pops the current state from the state stack.
     * 
     * Returns to the previous state. Used for resuming from pause.
     */
    void popState();

    /**
     * @brief Replaces the current state with a new one.
     * 
     * This clears the state stack and sets the new state as the
     * only state. Used for menu transitions.
     * 
     * @param state Unique pointer to the new state
     */
    void changeState(std::unique_ptr<GameState> state);

    /**
     * @brief Gets a pointer to the current state.
     * @return Pointer to the current state, or nullptr if no state exists
     */
    GameState* getCurrentState() const;

    /**
     * @brief Checks if the game is running.
     * @return True if the game is running, false otherwise
     */
    bool isRunning() const { return running_; }

    /**
     * @brief Gets the render window.
     * @return Reference to the render window
     */
    sf::RenderWindow& getWindow() { return window_; }

    /**
     * @brief Gets the window width.
     * @return Window width in pixels
     */
    unsigned int getWindowWidth() const { return WINDOW_WIDTH; }

    /**
     * @brief Gets the window height.
     * @return Window height in pixels
     */
    unsigned int getWindowHeight() const { return WINDOW_HEIGHT; }

private:
    // Window constants
    static constexpr unsigned int WINDOW_WIDTH = 1280;
    static constexpr unsigned int WINDOW_HEIGHT = 720;
    static constexpr const char* WINDOW_TITLE = "Cyberpunk Cannon Shooter";

    // SFML window
    sf::RenderWindow window_;

    // State stack
    std::vector<std::unique_ptr<GameState>> stateStack_;

    // Game loop control
    bool running_;
    sf::Clock clock_;  // For delta time calculation

    /**
     * @brief Handles window events.
     * 
     * Processes global window events like closing the window.
     */
    void handleWindowEvents();

    /**
     * @brief Updates the current state.
     * @param deltaTime Time elapsed since last frame (in seconds)
     */
    void update(float deltaTime);

    /**
     * @brief Renders the current state.
     */
    void render();
};

#endif // GAME_H

