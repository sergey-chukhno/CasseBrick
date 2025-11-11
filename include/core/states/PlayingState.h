#ifndef PLAYINGSTATE_H
#define PLAYINGSTATE_H

#include "core/GameState.h"
#include "entities/Cannon.h"
#include "entities/Projectile.h"
#include <SFML/Graphics.hpp>
#include <memory>

// Forward declaration
class Game;

/**
 * @brief Playing state for active gameplay.
 * 
 * PlayingState is the main gameplay state where the player controls
 * the cannon and shoots projectiles at blocks. Currently a placeholder
 * that will be implemented in later steps.
 */
class PlayingState : public GameState
{
public:
    /**
     * @brief Constructs a PlayingState.
     * @param game Pointer to the Game instance for state transitions
     */
    explicit PlayingState(Game* game);

    /**
     * @brief Destructor.
     */
    ~PlayingState() override = default;

    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    void handleEvent(const sf::Event& event) override;
    void onEnter() override;
    void onExit() override;

private:
    Game* game_;
    const sf::Font& font_;  // Reference to font from FontManager
    sf::Text placeholderText_;
    
    // Cannon
    std::unique_ptr<Cannon> cannon_;
    
    // Projectile pool
    ProjectilePool projectilePool_;
};

#endif // PLAYINGSTATE_H

